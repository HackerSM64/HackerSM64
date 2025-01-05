#include <cart.h>
#include "cartint.h"
#include "sc.h"

int sc_card_wr_dram(const void *dram, u32 lba, u32 count)
{
	const char *addr = dram;
	int i;
	int n;
	__cart_acs_get();
	__sc_sync();
	while (count > 0)
	{
		n = count < 16 ? count : 16;
		if ((long)addr & 7)
		{
			for (i = 0; i < n; i++)
			{
				__cart_buf_rd(addr);
				__cart_dma_wr(__cart_buf, SC_BUFFER_REG+512*i, 512);
				addr += 512;
			}
		}
		else
		{
			__cart_dma_wr(addr, SC_BUFFER_REG, 512*n);
			addr += 512*n;
		}
		__cart_wr(SC_DATA0_REG, lba);
		__cart_wr(SC_COMMAND_REG, SC_SD_SECTOR_SET);
		if (__sc_sync()) CART_ABORT();
		__cart_wr(SC_DATA0_REG, SC_BUFFER_REG);
		__cart_wr(SC_DATA1_REG, n);
		__cart_wr(SC_COMMAND_REG, SC_SD_WRITE);
		if (__sc_sync()) CART_ABORT();
		lba += n;
		count -= n;
	}
	__cart_acs_rel();
	return 0;
}
