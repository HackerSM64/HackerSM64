#include <cart.h>
#include "cartint.h"

int cart_card_rd_dram(void *dram, u32 lba, u32 count)
{
	static int (*const card_rd_dram[CART_MAX])(
		void *dram, u32 lba, u32 count
	) =
	{
		ci_card_rd_dram,
		edx_card_rd_dram,
		ed_card_rd_dram,
		sc_card_rd_dram,
	};
	if (cart_type < 0) return -1;
	return card_rd_dram[cart_type](dram, lba, count);
}
