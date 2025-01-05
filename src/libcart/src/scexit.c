#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_exit(void)
{
	__cart_acs_get();
	__sc_sync();
	__cart_wr(SC_DATA1_REG, SC_SD_DEINIT);
	__cart_wr(SC_COMMAND_REG, SC_SD_OP);
	__sc_sync();
	__cart_wr(SC_DATA0_REG, SC_CFG_ROM_WRITE);
	__cart_wr(SC_DATA1_REG, 0);
	__cart_wr(SC_COMMAND_REG, SC_CONFIG_SET);
	__sc_sync();
	__cart_wr(SC_KEY_REG, SC_KEY_RESET);
	__cart_wr(SC_KEY_REG, SC_KEY_LOCK);
	__cart_acs_rel();
	return 0;
}
