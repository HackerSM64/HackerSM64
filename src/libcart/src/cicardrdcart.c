#include <cart.h>
#include "cartint.h"
#include "ci.h"

int ci_card_rd_cart(u32 cart, u32 lba, u32 count)
{
	__cart_acs_get();
	__ci_sync();
	if (cart_card_byteswap)
	{
		__cart_wr(CI_COMMAND_REG, CI_BYTESWAP_ON);
		__ci_sync();
	}
	__cart_wr(CI_LBA_REG, lba);
	__cart_wr(CI_LENGTH_REG, count);
	__cart_wr(CI_SDRAM_ADDR_REG, (cart & 0xFFFFFFF) >> 1);
	__cart_wr(CI_COMMAND_REG, CI_RD_SDRAM);
	if (__ci_sync())
	{
		__cart_wr(CI_COMMAND_REG, CI_ABORT);
		__ci_sync();
		__cart_wr(CI_COMMAND_REG, CI_SD_RESET);
		__ci_sync();
		__cart_wr(CI_COMMAND_REG, CI_BYTESWAP_OFF);
		__ci_sync();
		CART_ABORT();
	}
	if (cart_card_byteswap)
	{
		__cart_wr(CI_COMMAND_REG, CI_BYTESWAP_OFF);
		__ci_sync();
	}
	__cart_acs_rel();
	return 0;
}
