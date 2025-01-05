#include <cart.h>
#include "cartint.h"
#include "ci.h"

int ci_exit(void)
{
	__cart_acs_get();
	__ci_sync();
	__cart_wr(CI_COMMAND_REG, CI_CARTROM_WR_OFF);
	__ci_sync();
	__cart_acs_rel();
	return 0;
}
