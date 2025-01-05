#include <cart.h>
#include "cartint.h"
#include "ed.h"

int ed_exit(void)
{
	__cart_acs_get();
	__cart_wr(ED_KEY_REG, 0);
	__cart_acs_rel();
	return 0;
}
