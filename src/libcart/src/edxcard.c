#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "edx.h"

void __edx_sd_mode(u32 reg, int val)
{
	static u32 mode;
	if (mode != reg)
	{
		mode = reg;
		__cart_wr(EDX_SD_STATUS_REG, __sd_cfg);
		__cart_wr(reg, 0xFFFF);
		while (__cart_rd(EDX_SD_STATUS_REG) & EDX_SD_STA_BUSY);
	}
	__cart_wr(EDX_SD_STATUS_REG, __sd_cfg | val);
}

u32 __edx_sd_cmd_rd(void)
{
	__cart_wr(EDX_SD_CMD_RD_REG, 0xFFFF);
	while (__cart_rd(EDX_SD_STATUS_REG) & EDX_SD_STA_BUSY);
	return __cart_rd(EDX_SD_CMD_RD_REG);
}

void __edx_sd_cmd_wr(u32 val)
{
	__cart_wr(EDX_SD_CMD_WR_REG, val);
	while (__cart_rd(EDX_SD_STATUS_REG) & EDX_SD_STA_BUSY);
}

u32 __edx_sd_dat_rd(void)
{
	__cart_wr(EDX_SD_DAT_RD_REG, 0xFFFF);
	return __cart_rd(EDX_SD_DAT_RD_REG);
}

int __edx_sd_cmd(int cmd, u32 arg)
{
	int i;
	int n;
	char buf[6];
	buf[0] = cmd;
	buf[1] = arg >> 24;
	buf[2] = arg >> 16;
	buf[3] = arg >>  8;
	buf[4] = arg >>  0;
	buf[5] = __sd_crc7(buf);
	/* Send the command */
	__edx_sd_mode(EDX_SD_CMD_WR, EDX_SD_CMD_8b);
	__edx_sd_cmd_wr(0xFF);
	for (i = 0; i < 6; i++) __edx_sd_cmd_wr(buf[i] & 0xFF);
	if (cmd == CMD18) return 0;
	/* Read the first response byte */
	__edx_sd_mode(EDX_SD_CMD_RD, EDX_SD_CMD_8b);
	__sd_resp[0] = __edx_sd_cmd_rd();
	__edx_sd_mode(EDX_SD_CMD_RD, EDX_SD_CMD_1b);
	n = 2048;
	while (__sd_resp[0] & 0xC0)
	{
		if (--n == 0) return -1;
		__sd_resp[0] = __edx_sd_cmd_rd();
	}
	/* Read the rest of the response */
	__edx_sd_mode(EDX_SD_CMD_RD, EDX_SD_CMD_8b);
	n = cmd == CMD2 || cmd == CMD9 ? 17 : 6;
	for (i = 1; i < n; i++) __sd_resp[i] = __edx_sd_cmd_rd();
	return 0;
}

int __edx_sd_close(void)
{
	int n;
	/* CMD12: STOP_TRANSMISSION */
	if (__edx_sd_cmd(CMD12, 0) < 0) return -1;
	/* Wait for card */
	__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_4b);
	__edx_sd_dat_rd();
	__edx_sd_mode(EDX_SD_DAT_RD, EDX_SD_DAT_8b);
	__edx_sd_dat_rd();
	n = 65536;
	do
	{
		if (--n == 0) break;
	}
	while ((__edx_sd_dat_rd() & 0xFF) != 0xFF);
	return 0;
}
