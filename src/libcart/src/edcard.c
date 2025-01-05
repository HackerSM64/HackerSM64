#include <cart.h>
#include "cartint.h"
#include "sd.h"
#include "ed.h"

/* SPI exchange */
int __ed_spi(int val)
{
	__cart_wr(ED_SPI_REG, val);
	while (__cart_rd(ED_STATUS_REG) & ED_STATE_SPI);
	return __cart_rd(ED_SPI_REG);
}

int __ed_sd_cmd(int cmd, u32 arg)
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
	__ed_sd_mode(ED_SD_CMD_WR, ED_SD_CMD_8b);
	__ed_sd_cmd_wr(0xFF);
	for (i = 0; i < 6; i++) __ed_sd_cmd_wr(buf[i]);
	/* Read the first response byte */
	__sd_resp[0] = 0xFF;
	__ed_sd_mode(ED_SD_CMD_RD, ED_SD_CMD_1b);
	n = 2048;
	while (__sd_resp[0] & 0xC0)
	{
		if (--n == 0) return -1;
		__sd_resp[0] = __ed_sd_cmd_rd(__sd_resp[0]);
	}
	/* Read the rest of the response */
	n = !__sd_type ?
		cmd == CMD8 || cmd == CMD58 ? 5 : 1 :
		cmd == CMD2 || cmd == CMD9 ? 17 : 6;
	__ed_sd_mode(ED_SD_CMD_RD, ED_SD_CMD_8b);
	for (i = 1; i < n; i++) __sd_resp[i] = __ed_sd_cmd_rd(0xFF);
	/* SPI: return "illegal command" flag */
	return !__sd_type ? (__sd_resp[0] & 4) : 0;
}

int __ed_sd_close(int flag)
{
	int n;
	if (!flag)
	{
		/* SPI: Stop token (write) */
		__ed_sd_mode(ED_SD_DAT_WR, ED_SD_DAT_8b);
		__ed_sd_dat_wr(0xFD);
		__ed_sd_dat_wr(0xFF);
	}
	else
	{
		/* CMD12: STOP_TRANSMISSION */
		if (__ed_sd_cmd(CMD12, 0) < 0) return -1;
	}
	/* Wait for card */
	__ed_sd_mode(ED_SD_DAT_RD, ED_SD_DAT_8b);
	n = 65536;
	do
	{
		if (--n == 0) break;
	}
	while ((__ed_sd_dat_rd() & 0xFF) != 0xFF);
	return 0;
}
