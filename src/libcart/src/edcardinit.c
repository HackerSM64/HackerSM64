#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "ed.h"

int ed_card_init(void)
{
	int i;
	int n;
	u32 rca;
	__cart_acs_get();
	/* Detect SD interface */
	/* 0: use SPI */
	/* 1: use SD */
	__sd_type = 0;
	if ((__cart_rd(ED_VER_REG) & 0xFFFF) >= 0x116)
	{
		/* Check bootloader ROM label for "ED64 SD boot" */
		__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_OFF);
		/* label[4:8] == " SD " */
		if (__cart_rd(0x10000024) == 0x20534420) __sd_type = 1;
		__cart_wr(ED_CFG_REG, ED_CFG_SDRAM_ON);
	}
	/* SPI: SS = 0 */
	/* SD : SS = 1 */
	__sd_cfg = ED_SPI_SPD_LO;
	if (__sd_type) __sd_cfg |= ED_SPI_SS;
	/* Card needs 74 clocks, we do 80 */
	__ed_sd_mode(ED_SD_CMD_WR, ED_SD_CMD_8b);
	for (i = 0; i < 10; i++) __ed_sd_cmd_wr(0xFF);
	/* CMD0: GO_IDLE_STATE */
	__ed_sd_cmd(CMD0, 0);
	/* CMD8: SEND_IF_COND */
	/* If it returns an error, it is SD V1 */
	if (__ed_sd_cmd(CMD8, 0x1AA))
	{
		/* SD V1 */
		if (!__sd_type)
		{
			if (__ed_sd_cmd(CMD55, 0) < 0) CART_ABORT();
			if (__ed_sd_cmd(ACMD41, 0x40300000) < 0)
			{
				n = 1024;
				do
				{
					if (--n == 0) CART_ABORT();
					if (__ed_sd_cmd(CMD1, 0) < 0) CART_ABORT();
				}
				while (__sd_resp[0] != 0);
			}
			else
			{
				n = 1024;
				do
				{
					if (--n == 0) CART_ABORT();
					if (__ed_sd_cmd(CMD55, 0) < 0) CART_ABORT();
					if (__sd_resp[0] != 1) continue;
					if (__ed_sd_cmd(ACMD41, 0x40300000) < 0) CART_ABORT();
				}
				while (__sd_resp[0] != 0);
			}
		}
		else
		{
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
				if (__ed_sd_cmd(CMD55, 0) < 0) CART_ABORT();
				if (__ed_sd_cmd(ACMD41, 0x40300000) < 0) CART_ABORT();
			}
			while (__sd_resp[1] == 0);
		}
		__sd_flag = 0;
	}
	else
	{
		/* SD V2 */
		if (!__sd_type)
		{
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
				if (__ed_sd_cmd(CMD55, 0) < 0) CART_ABORT();
				if (__sd_resp[0] != 1) continue;
				if (__ed_sd_cmd(ACMD41, 0x40300000) < 0) CART_ABORT();
			}
			while (__sd_resp[0] != 0);
			if (__ed_sd_cmd(CMD58, 0) < 0) CART_ABORT();
		}
		else
		{
			n = 1024;
			do
			{
				if (--n == 0) CART_ABORT();
				if (__ed_sd_cmd(CMD55, 0) < 0) CART_ABORT();
				if (!(__sd_resp[3] & 1)) continue;
				__ed_sd_cmd(ACMD41, 0x40300000);
			}
			while (!(__sd_resp[1] & 0x80));
		}
		/* Card is SDHC */
		__sd_flag = __sd_resp[1] & 0x40;
	}
	if (!__sd_type)
	{
		__sd_cfg = ED_SPI_SPD_25;
	}
	else
	{
		/* CMD2: ALL_SEND_CID */
		if (__ed_sd_cmd(CMD2, 0) < 0) CART_ABORT();
		/* CMD3: SEND_RELATIVE_ADDR */
		if (__ed_sd_cmd(CMD3, 0) < 0) CART_ABORT();
		rca =
			__sd_resp[1] << 24 |
			__sd_resp[2] << 16 |
			__sd_resp[3] <<  8 |
			__sd_resp[4] <<  0;
		/* CMD9: SEND_CSD */
		if (__ed_sd_cmd(CMD9, rca) < 0) CART_ABORT();
		/* CMD7: SELECT_CARD */
		if (__ed_sd_cmd(CMD7, rca) < 0) CART_ABORT();
		/* ACMD6: SET_BUS_WIDTH */
		if (__ed_sd_cmd(CMD55, rca) < 0) CART_ABORT();
		if (__ed_sd_cmd(ACMD6, 2) < 0) CART_ABORT();
		__sd_cfg = ED_SPI_SPD_50|ED_SPI_SS;
	}
	__cart_acs_rel();
	return 0;
}
