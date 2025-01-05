#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_card_rd_cart(u32 cart, u32 lba, u32 count)
{
	__cart_acs_get();
	__sc_sync();
	if (cart_card_byteswap)
	{
		__cart_wr(SC_DATA1_REG, SC_SD_BYTESWAP_ON);
		__cart_wr(SC_COMMAND_REG, SC_SD_OP);
		if (__sc_sync()) CART_ABORT();
	}
	__cart_wr(SC_DATA0_REG, lba);
	__cart_wr(SC_COMMAND_REG, SC_SD_SECTOR_SET);
	if (__sc_sync()) CART_ABORT();
	__cart_wr(SC_DATA0_REG, cart);
	__cart_wr(SC_DATA1_REG, count);
	__cart_wr(SC_COMMAND_REG, SC_SD_READ);
	if (__sc_sync()) CART_ABORT();
	if (cart_card_byteswap)
	{
		__cart_wr(SC_DATA1_REG, SC_SD_BYTESWAP_OFF);
		__cart_wr(SC_COMMAND_REG, SC_SD_OP);
		if (__sc_sync()) CART_ABORT();
	}
	__cart_acs_rel();
	return 0;
}
