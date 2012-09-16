/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <inttypes.h>
#include "mmc_if.h"

/* globals */
DSTATUS SD_STATUS = STA_NODISK | STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	if (drv)
		return STA_NOINIT;
	mmc_init();
	SD_STATUS &= ~STA_NODISK;
	SD_STATUS &= ~STA_NOINIT;
	return SD_STATUS; /* TODO: verify if this is success */
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	if (drv)
		return STA_NOINIT;
	return SD_STATUS;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (SD_STATUS & STA_NOINIT) return RES_NOTRDY;

	if (count > 1)
		return RES_PARERR;

	if (mmc_readsector(sector, buff))
		return RES_PARERR;
	else
		return RES_OK;

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (SD_STATUS & STA_NOINIT) return RES_NOTRDY;

	if (count > 1)
		return RES_PARERR;

	if (mmc_writesector(sector, buff))
		return RES_PARERR;
	else
		return RES_OK;

}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch(ctrl)
	{
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT:
			*(DWORD *)buff = 4194304; /* 2GB SD card */
			res = RES_OK;
			break;
		 case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
		         *(WORD*)buff = 512;
		         res = RES_OK;
		         break;
		default:
			res = RES_PARERR;
		        break;
	}

	return res;
}

