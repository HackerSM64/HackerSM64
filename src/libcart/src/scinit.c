#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_init(void)
{
	u32 cfg;
	__cart_acs_get();
	__cart_wr(SC_KEY_REG, SC_KEY_RESET);
	__cart_wr(SC_KEY_REG, SC_KEY_UNL);
	__cart_wr(SC_KEY_REG, SC_KEY_OCK);
	if (__cart_rd(SC_IDENTIFIER_REG) != SC_IDENTIFIER) CART_ABORT();
	__sc_sync();
	__cart_wr(SC_DATA0_REG, SC_CFG_ROM_WRITE);
	__cart_wr(SC_DATA1_REG, 1);
	__cart_wr(SC_COMMAND_REG, SC_CONFIG_SET);
	__sc_sync();
	/* SC64 uses SDRAM for 64DD */
	__cart_wr(SC_DATA0_REG, SC_CFG_DD_MODE);
	__cart_wr(SC_COMMAND_REG, SC_CONFIG_GET);
	__sc_sync();
	cfg = __cart_rd(SC_DATA1_REG);
	/* Have registers */
	if (cfg & SC_DD_MODE_REGS)
	{
		cart_size = 0x2000000; /* 32 MiB */
	}
	/* Have IPL */
	else if (cfg & SC_DD_MODE_IPL)
	{
		cart_size = 0x3BC0000; /* 59.75 MiB */
	}
	else
	{
		/* SC64 does not have physical SRAM on board */
		/* The end of SDRAM is used for SRAM or FlashRAM save types */
		__cart_wr(SC_DATA0_REG, SC_CFG_SAVE_TYPE);
		__cart_wr(SC_COMMAND_REG, SC_CONFIG_GET);
		__sc_sync();
		/* Have SRAM or FlashRAM */
		if (__cart_rd(SC_DATA1_REG) >= 3)
		{
			cart_size = 0x3FE0000; /* 64 MiB - 128 KiB */
		}
		else
		{
			cart_size = 0x4000000; /* 64 MiB */
		}
	}
	__cart_acs_rel();
	return 0;
}
