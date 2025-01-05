#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "ed.h"

int ed_card_wr_dram(const void *dram, u32 lba, u32 count)
{
	const char *addr = dram;
	int i;
	int n;
	int resp;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD25: WRITE_MULTIPLE_BLOCK */
	if (__ed_sd_cmd(CMD25, lba) < 0) CART_ABORT();
	if (!__sd_type)
	{
		/* SPI: padding (why 2 bytes?) */
		__ed_sd_mode(ED_SD_DAT_WR, ED_SD_DAT_8b);
		__ed_sd_dat_wr(0xFF);
		__ed_sd_dat_wr(0xFF);
	}
	while (count-- > 0)
	{
		__ed_sd_mode(ED_SD_DAT_WR, ED_SD_DAT_8b);
		if (!__sd_type)
		{
			/* SPI: data token */
			__ed_sd_dat_wr(0xFC);
		}
		else
		{
			/* SD: start bit (why not only write F0?) */
			__ed_sd_dat_wr(0xFF);
			__ed_sd_dat_wr(0xF0);
		}
		/* Write data */
		for (i = 0; i < 512; i++) __ed_sd_dat_wr(addr[i]);
		if (!__sd_type)
		{
			/* SPI: write dummy CRC */
			for (i = 0; i < 2; i++) __ed_sd_dat_wr(0xFF);
		}
		else
		{
			/* SD: write real CRC */
			if ((long)addr & 7)
			{
				__cart_buf_rd(addr);
				__sd_crc16(__cart_buf, __cart_buf);
			}
			else
			{
				__sd_crc16(__cart_buf, (const u64 *)addr);
			}
			for (i = 0; i < 8; i++) __ed_sd_dat_wr(((char *)__cart_buf)[i]);
			/* End bit */
			__ed_sd_mode(ED_SD_DAT_WR, ED_SD_DAT_1b);
			__ed_sd_dat_wr(0xFF);
			/* Wait for start of response */
			__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_1b);
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
			}
			while (__ed_sd_dat_rd() & 1);
			/* Read response */
			resp = 0;
			for (i = 0; i < 3; i++) resp = resp << 1 | (__ed_sd_dat_rd() & 1);
			if (resp != 2) CART_ABORT();
		}
		/* Wait for card */
		__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_8b);
		n = 65536;
		do
		{
			if (--n == 0) CART_ABORT();
		}
		while ((__ed_sd_dat_rd() & 0xFF) != 0xFF);
		addr += 512;
	}
	if (__ed_sd_close(__sd_type)) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
