#include <cart.h>
#include "cartint.h"

int cart_exit(void)
{
	static int (*const exit[CART_MAX])(void) =
	{
		ci_exit,
		edx_exit,
		ed_exit,
		sc_exit,
	};
	if (cart_type < 0) return -1;
	return exit[cart_type]();
}
