#include <cart.h>
#include "cartint.h"
#include "sc.h"

int __sc_sync(void)
{
	while (__cart_rd(SC_STATUS_REG) & SC_CMD_BUSY);
	if (__cart_rd(SC_STATUS_REG) & SC_CMD_ERROR) return -1;
	return 0;
}
