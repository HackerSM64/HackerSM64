#include <cart.h>
#include "cartint.h"
#include "edx.h"

int edx_exit(void)
{
	__cart_acs_get();
	__cart_wr(EDX_KEY_REG, 0);
	__cart_acs_rel();
	return 0;
}
