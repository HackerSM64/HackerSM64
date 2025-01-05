/*-----------------------------------------------------------------------*/
/* Low level disk I/O module for FatFs (libcart)    (C)ChaN, 2019        */
/*                                             (C)devwizard, 2022-2023   */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#ifdef _ULTRA64
#include <ultra64.h>
#else
#include <libdragon.h>
#endif
#include <cart.h>

/* Definitions of physical drive number for each drive */
#define DEV_CARD	0	/* Cartridge memory card */


static DSTATUS card_status;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_CARD :
		return card_status;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_CARD :
		return card_status = cart_card_init() ? STA_NOINIT : 0;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u32 addr;

	switch (pdrv) {
	case DEV_CARD :
#ifdef _ULTRA64
		addr = osVirtualToPhysical(buff);
#else
		addr = PhysicalAddr(buff);
#endif
		if (addr < 0x800000) return cart_card_rd_dram(buff, sector, count) ? RES_ERROR : RES_OK;
		else                 return cart_card_rd_cart(addr, sector, count) ? RES_ERROR : RES_OK;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u32 addr;

	switch (pdrv) {
	case DEV_CARD :
#ifdef _ULTRA64
		addr = osVirtualToPhysical((void *)buff);
#else
		addr = PhysicalAddr(buff);
#endif
		if (addr < 0x800000) return cart_card_wr_dram(buff, sector, count) ? RES_ERROR : RES_OK;
		else                 return cart_card_wr_cart(addr, sector, count) ? RES_ERROR : RES_OK;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	(void)buff;

	switch (pdrv) {
	case DEV_CARD :
		switch (cmd) {
#if !FF_FS_READONLY
		case CTRL_SYNC:
			return RES_OK;
#endif

#if !FF_FS_READONLY && FF_USE_MKFS
#if 0
		case GET_SECTOR_COUNT:
			*(LBA_t *)buff = 0;
			break;
#endif
#endif

#if FF_MAX_SS != FF_MIN_SS
		case GET_SECTOR_SIZE:
			*(WORD *)buff = 512;
			return RES_OK;
#endif

#if !FF_FS_READONLY && FF_USE_MKFS
		case GET_BLOCK_SIZE:
			*(DWORD *)buff = 512;
			return RES_OK;
#endif
		}
	}

	return RES_PARERR;
}


#if !FF_FS_READONLY && !FF_FS_NORTC

DWORD get_fattime (void)
{
	return 0;
}

#endif

