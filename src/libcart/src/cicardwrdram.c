#include <cart.h>
#include "cartint.h"
#include "ci.h"

int ci_card_wr_dram(const void *dram, u32 lba, u32 count)
{
	const char *addr = dram;
	__cart_acs_get();
	__ci_sync();
	while (count-- > 0)
	{
		if ((long)addr & 7)
		{
			__cart_buf_rd(addr);
			__cart_dma_wr(__cart_buf, CI_BUFFER_REG, 512);
		}
		else
		{
			__cart_dma_wr(addr, CI_BUFFER_REG, 512);
		}
		__cart_wr(CI_LBA_REG, lba);
		__cart_wr(CI_COMMAND_REG, CI_WR_BUFFER);
		if (__ci_sync())
		{
			__cart_wr(CI_COMMAND_REG, CI_ABORT);
			__ci_sync();
			__cart_wr(CI_COMMAND_REG, CI_SD_RESET);
			__ci_sync();
			CART_ABORT();
		}
		addr += 512;
		lba++;
	}
	__cart_acs_rel();
	return 0;
}
