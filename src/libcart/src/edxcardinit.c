#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "edx.h"

int edx_card_init(void)
{
	int i;
	int n;
	uint32_t rca;
	uint32_t boot_cfg;
	__cart_acs_get();
	/* Check if already init */
	boot_cfg = __cart_rd(EDX_BOOT_CFG_REG);
	if (boot_cfg & EDX_BCFG_SD_INIT)
	{
		__sd_flag = boot_cfg & EDX_BCFG_SD_TYPE;
	}
	else
	{
		__sd_cfg = 0;
		/* Card needs 74 clocks, we do 80 */
		__edx_sd_mode(EDX_SD_CMD_WR, EDX_SD_CMD_8b);
		for (i = 0; i < 10; i++) __edx_sd_cmd_wr(0xFF);
		/* CMD0: GO_IDLE_STATE */
		__edx_sd_cmd(CMD0, 0);
		/* CMD8: SEND_IF_COND */
		/* If it returns an error, it is SD V1 */
		if (__edx_sd_cmd(CMD8, 0x1AA))
		{
			/* SD V1 */
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
				if (__edx_sd_cmd(CMD55, 0) < 0) CART_ABORT();
				if (__edx_sd_cmd(ACMD41, 0x40300000) < 0) CART_ABORT();
			}
			while (__sd_resp[1] == 0);
			__sd_flag = 0;
		}
		else
		{
			/* SD V2 */
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
				if (__edx_sd_cmd(CMD55, 0) < 0) CART_ABORT();
				if (!(__sd_resp[3] & 1)) continue;
				__edx_sd_cmd(ACMD41, 0x40300000);
			}
			while (!(__sd_resp[1] & 0x80));
			/* Card is SDHC */
			__sd_flag = __sd_resp[1] & 0x40;
		}
		/* CMD2: ALL_SEND_CID */
		if (__edx_sd_cmd(CMD2, 0) < 0) CART_ABORT();
		/* CMD3: SEND_RELATIVE_ADDR */
		if (__edx_sd_cmd(CMD3, 0) < 0) CART_ABORT();
		rca =
			__sd_resp[1] << 24 |
			__sd_resp[2] << 16 |
			__sd_resp[3] <<  8 |
			__sd_resp[4] <<  0;
		/* CMD9: SEND_CSD */
		if (__edx_sd_cmd(CMD9, rca) < 0) CART_ABORT();
		/* CMD7: SELECT_CARD */
		if (__edx_sd_cmd(CMD7, rca) < 0) CART_ABORT();
		/* ACMD6: SET_BUS_WIDTH */
		if (__edx_sd_cmd(CMD55, rca) < 0) CART_ABORT();
		if (__edx_sd_cmd(ACMD6, 2) < 0) CART_ABORT();
	}
	__sd_cfg = EDX_SD_CFG_SPD;
	__cart_acs_rel();
	return 0;
}
