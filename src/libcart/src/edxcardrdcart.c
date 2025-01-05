#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "edx.h"

int edx_card_rd_cart(u32 cart, u32 lba, u32 count)
{
	u32 resp;
	__cart_acs_get();
	/* SDSC takes byte address, SDHC takes LBA */
	if (!__sd_flag) lba *= 512;
	/* CMD18: READ_MULTIPLE_BLOCK */
	if (__edx_sd_cmd(CMD18, lba) < 0) CART_ABORT();
	if (cart_card_byteswap)
	{
		__cart_wr(EDX_SYS_CFG_REG, EDX_CFG_SDRAM_ON|EDX_CFG_BYTESWAP);
	}
	__cart_wr(EDX_DMA_ADDR_REG, cart & 0x3FFFFFF);
	__cart_wr(EDX_DMA_LEN_REG, count);
	__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_16b);
	while ((resp = __cart_rd(EDX_DMA_STA_REG)) & EDX_DMA_STA_BUSY)
	{
		if (resp & EDX_DMA_STA_ERROR)
		{
			__cart_wr(EDX_SYS_CFG_REG, EDX_CFG_SDRAM_ON);
			CART_ABORT();
		}
	}
	if (cart_card_byteswap)
	{
		__cart_wr(EDX_SYS_CFG_REG, EDX_CFG_SDRAM_ON);
	}
	if (__edx_sd_close()) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
