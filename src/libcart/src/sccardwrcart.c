#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_card_wr_cart(u32 cart, u32 lba, u32 count)
{
	__cart_acs_get();
	__sc_sync();
	__cart_wr(SC_DATA0_REG, lba);
	__cart_wr(SC_COMMAND_REG, SC_SD_SECTOR_SET);
	if (__sc_sync()) CART_ABORT();
	__cart_wr(SC_DATA0_REG, cart);
	__cart_wr(SC_DATA1_REG, count);
	__cart_wr(SC_COMMAND_REG, SC_SD_WRITE);
	if (__sc_sync()) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
