#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "ed.h"

int ed_card_rd_dram(void *dram, u32 lba, u32 count)
{
	char *addr = dram;
	int i;
	int n;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD18: READ_MULTIPLE_BLOCK */
	if (__ed_sd_cmd(CMD18, lba) < 0) CART_ABORT();
	while (count-- > 0)
	{
		/* Wait for card */
		__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_1b);
		n = 65536;
		do
		{
			if (--n == 0) CART_ABORT();
		}
		while (__ed_sd_dat_rd() & 1);
		/* Read data */
		__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_8b);
		for (i = 0; i < 512; i++) addr[i] = __ed_sd_dat_rd();
		/* SPI: 1x16-bit CRC (2 byte) */
		/* SD:  4x16-bit CRC (8 byte) */
		/* We ignore the CRC */
		n = !__sd_type ? 2 : 8;
		for (i = 0; i < n; i++) __ed_sd_dat_rd();
		addr += 512;
	}
	if (__ed_sd_close(1)) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
