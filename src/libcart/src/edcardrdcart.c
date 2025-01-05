#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "ed.h"

int ed_card_rd_cart(u32 cart, u32 lba, u32 count)
{
	int i;
	int n;
	u32 resp;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD18: READ_MULTIPLE_BLOCK */
	if (__ed_sd_cmd(CMD18, lba) < 0) CART_ABORT();
	/* DMA requires 2048-byte alignment */
	if (cart & 0x7FF)
	{
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
			for (i = 0; i < 512; i++)
			{
				((char *)__cart_buf)[i] = __ed_sd_dat_rd();
			}
			/* SPI: 1x16-bit CRC (2 byte) */
			/* SD:  4x16-bit CRC (8 byte) */
			/* We ignore the CRC */
			n = !__sd_type ? 2 : 8;
			for (i = 0; i < n; i++) __ed_sd_dat_rd();
			__cart_dma_wr(__cart_buf, cart, 512);
			cart += 512;
		}
	}
	else
	{
		if (cart_card_byteswap)
		{
			__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_ON|ED_CFG_BYTESWAP);
		}
		__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_8b);
		__cart_wr(ED_DMA_LEN_REG, count-1);
		__cart_wr(ED_DMA_ADDR_REG, (cart & 0x3FFFFFF) >> 11);
		__cart_wr(ED_DMA_CFG_REG, ED_DMA_SD_TO_RAM);
		while ((resp = __cart_rd(ED_STATUS_REG)) & ED_STATE_DMA_BUSY)
		{
			if (resp & ED_STATE_DMA_TOUT)
			{
				__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_ON);
				CART_ABORT();
			}
		}
		if (cart_card_byteswap)
		{
			__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_ON);
		}
	}
	if (__ed_sd_close(1)) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
