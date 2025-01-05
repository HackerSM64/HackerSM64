#include <cart.h>
#include "cartint.h"

char cart_card_byteswap;

int cart_card_rd_cart(u32 cart, u32 lba, u32 count)
{
	static int (*const card_rd_cart[CART_MAX])(
		u32 cart, u32 lba, u32 count
	) =
	{
		ci_card_rd_cart,
		edx_card_rd_cart,
		ed_card_rd_cart,
		sc_card_rd_cart,
	};
	if (cart_type < 0) return -1;
	return card_rd_cart[cart_type](cart, lba, count);
}
