#include <cart.h>
#include "cartint.h"
#include "ci.h"

int ci_init(void)
{
	__cart_acs_get();
	if (__cart_rd(CI_MAGIC_REG) != CI_MAGIC) CART_ABORT();
	__ci_sync();
	__cart_wr(CI_COMMAND_REG, CI_CARTROM_WR_ON);
	__ci_sync();
	__cart_wr(CI_COMMAND_REG, CI_BYTESWAP_OFF);
	__ci_sync();
	cart_size = 0x4000000; /* 64 MiB */
	__cart_acs_rel();
	return 0;
}
