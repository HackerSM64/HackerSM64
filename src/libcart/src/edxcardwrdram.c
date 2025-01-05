#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "edx.h"

int edx_card_wr_dram(const void *dram, u32 lba, u32 count)
{
	const char *addr = dram;
	int i;
	int n;
	int resp;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD25: WRITE_MULTIPLE_BLOCK */
	if (__edx_sd_cmd(CMD25, lba) < 0) CART_ABORT();
	while (count-- > 0)
	{
		/* SD: start bit (why not only write F0?) */
		__edx_sd_mode(EDX_SD_DAT_WR, EDX_SD_DAT_8b);
		__edx_sd_dat_wr(0xFF);
		__edx_sd_dat_wr(0xF0);
		/* Write data and CRC */
		__edx_sd_mode(EDX_SD_DAT_WR, EDX_SD_DAT_16b);
		if ((long)addr & 7)
		{
			__cart_buf_rd(addr);
			__cart_dma_wr(__cart_buf, EDX_SDIO_ARD_REG, 512);
			__sd_crc16(__cart_buf, __cart_buf);
		}
		else
		{
			__cart_dma_wr(addr, EDX_SDIO_ARD_REG, 512);
			__sd_crc16(__cart_buf, (const u64 *)addr);
		}
		__cart_dma_wr(__cart_buf, EDX_SDIO_ARD_REG, 8);
		/* End bit */
		__edx_sd_mode(EDX_SD_DAT_WR, EDX_SD_DAT_4b);
		__edx_sd_dat_wr(0xFF);
		/* Wait for start of response */
		__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_4b);
		n = 1024;
		do
		{
			if (--n == 0) CART_ABORT();
		}
		while (__edx_sd_dat_rd() & 1);
		/* Read response */
		resp = 0;
		for (i = 0; i < 3; i++) resp = resp << 1 | (__edx_sd_dat_rd() & 1);
		if (resp != 2) CART_ABORT();
		/* Wait for card */
		n = 65536;
		do
		{
			if (--n == 0) CART_ABORT();
		}
		while ((__edx_sd_dat_rd() & 0xFF) != 0xFF);
		addr += 512;
	}
	if (__edx_sd_close()) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
