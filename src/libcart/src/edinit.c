#include <cart.h>
#include "cartint.h"
#include "ed.h"

int ed_init(void)
{
	u32 ver;
	__cart_acs_get();
	__cart_wr(ED_KEY_REG, ED_KEY);
	ver = __cart_rd(ED_VER_REG) & 0xFFFF;
	if (ver < 0x100 || ver >= 0x400) CART_ABORT();
	__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_ON);
	__cart_dom2 = 0x80370404;
	/* V1/V2/V2.5 do not have physical SRAM on board */
	/* The end of SDRAM is used for SRAM or FlashRAM save types */
	if (ver < 0x300)
	{
		u32 sav = __cart_rd(ED_SAV_CFG_REG);
		/* Have 1M SRAM or FlashRAM */
		if (sav & ED_SAV_SRM_SIZE)
		{
			cart_size = 0x3FE0000; /* 64 MiB - 128 KiB */
		}
		/* Have 256K SRAM */
		else if (sav & ED_SAV_SRM_ON)
		{
			cart_size = 0x3FF8000; /* 64 MiB - 32KiB */
		}
		else
		{
			cart_size = 0x4000000; /* 64 MiB */
		}
	}
	__cart_acs_rel();
	return 0;
}
