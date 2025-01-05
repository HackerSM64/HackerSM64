#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_card_init(void)
{
	__cart_acs_get();
	__sc_sync();
	__cart_wr(SC_DATA1_REG, SC_SD_INIT);
	__cart_wr(SC_COMMAND_REG, SC_SD_OP);
	if (__sc_sync()) CART_ABORT();
	__cart_acs_rel();
	return 0;
}
