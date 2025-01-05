#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "edx.h"

int edx_card_rd_dram(void *dram, u32 lba, u32 count)
{
	char *addr = dram;
	int n;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD18: READ_MULTIPLE_BLOCK */
	if (__edx_sd_cmd(CMD18, lba) < 0) CART_ABORT();
	while (count-- > 0)
	{
		/* Wait for card */
		__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_4b);
		n = 65536;
		do
		{
			if (--n == 0) CART_ABORT();
		}
		while (__edx_sd_dat_rd() & 0xF);
		/* Read data */
		__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_16b);
		if ((long)addr & 7)
		{
			__cart_dma_rd(__cart_buf, EDX_SDIO_ARD_REG, 512);
			__cart_buf_wr(addr);
		}
		else
		{
			__cart_dma_rd(addr, EDX_SDIO_ARD_REG, 512);
		}
		/* 4x16-bit CRC (8 byte) */
		/* We ignore the CRC */
		__cart_dma_rd(__cart_buf, EDX_SDIO_ARD_REG, 8);
		addr += 512;
	}
	if (__edx_sd_close()) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
