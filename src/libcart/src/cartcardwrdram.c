#include <cart.h>
#include "cartint.h"

int cart_card_wr_dram(const void *dram, u32 lba, u32 count)
{
	static int (*const card_wr_dram[CART_MAX])(
		const void *dram, u32 lba, u32 count
	) =
	{
		ci_card_wr_dram,
		edx_card_wr_dram,
		ed_card_wr_dram,
		sc_card_wr_dram,
	};
	if (cart_type < 0) return -1;
	return card_wr_dram[cart_type](dram, lba, count);
}
