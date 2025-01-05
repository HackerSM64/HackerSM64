#include <cart.h>
#include "cartint.h"
#include "sd.h"

unsigned char __sd_resp[17];
unsigned char __sd_cfg;
unsigned char __sd_type;
unsigned char __sd_flag;

int __sd_crc7(const char *src)
{
	int i;
	int n;
	int crc = 0;
	for (i = 0; i < 5; i++)
	{
		crc ^= src[i];
		for (n = 0; n < 8; n++)
		{
			if ((crc <<= 1) & 0x100) crc ^= 0x12;
		}
	}
	return (crc & 0xFE) | 1;
}
