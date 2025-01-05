/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/

#include "ff.h"


#if FF_USE_LFN == 3	/* Use dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate/Free a Memory Block                                           */
/*------------------------------------------------------------------------*/

#include <stdlib.h>		/* with POSIX API */


void* ff_memalloc (	/* Returns pointer to the allocated memory block (null if not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
	return malloc((size_t)msize);	/* Allocate a new memory block */
}


void ff_memfree (
	void* mblock	/* Pointer to the memory block to free (no effect if null) */
)
{
	free(mblock);	/* Free the memory block */
}

#endif




#if FF_FS_REENTRANT	/* Mutal exclusion */
/*------------------------------------------------------------------------*/
/* Definitions of Mutex                                                   */
/*------------------------------------------------------------------------*/

#define OS_TYPE	0	/* 0:libultra */


#if   OS_TYPE == 0	/* libultra */
#include <ultra64.h>
static struct {
	OSMesgQueue mq;
	OSMesg msg[1];
} Mutex[FF_VOLUMES + 1];	/* Table of mutex access queue */

#endif



/*------------------------------------------------------------------------*/
/* Create a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to create a new mutex
/  or semaphore for the volume. When a 0 is returned, the f_mount function
/  fails with FR_INT_ERR.
*/

int ff_mutex_create (	/* Returns 1:Function succeeded or 0:Could not create the mutex */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* libultra */
	osCreateMesgQueue(&Mutex[vol].mq, Mutex[vol].msg, 1);
	osSendMesg(&Mutex[vol].mq, (OSMesg)0, OS_MESG_NOBLOCK);
	return 1;

#endif
}


/*------------------------------------------------------------------------*/
/* Delete a Mutex                                                         */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount function to delete a mutex or
/  semaphore of the volume created with ff_mutex_create function.
*/

void ff_mutex_delete (	/* Returns 1:Function succeeded or 0:Could not delete due to an error */
	int vol				/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* libultra */
	(void)vol;
	return 1;

#endif
}


/*------------------------------------------------------------------------*/
/* Request a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on enter file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_mutex_take (	/* Returns 1:Succeeded or 0:Timeout */
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* libultra */
#if FF_FS_TIMEOUT < 0
	osRecvMesg(&Mutex[vol].mq, NULL, OS_MESG_BLOCK);
#endif
#if FF_FS_TIMEOUT == 0
	if (osRecvMesg(&Mutex[vol].mq, NULL, OS_MESG_NOBLOCK) < 0) return 0;
#endif
#if FF_FS_TIMEOUT > 0
	OSMesg msg;
	OSTimer timer;
	osSetTimer(&timer, FF_FS_TIMEOUT, 0, &Mutex[vol].mq, (OSMesg)1);
	osRecvMesg(&Mutex[vol].mq, &msg, OS_MESG_BLOCK);
	osStopTimer(&timer);
	if (msg) return 0;
	osRecvMesg(&Mutex[vol].mq, NULL, OS_MESG_NOBLOCK);
#endif
	return 1;

#endif
}



/*------------------------------------------------------------------------*/
/* Release a Grant to Access the Volume                                   */
/*------------------------------------------------------------------------*/
/* This function is called on leave file functions to unlock the volume.
*/

void ff_mutex_give (
	int vol			/* Mutex ID: Volume mutex (0 to FF_VOLUMES - 1) or system mutex (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* libultra */
	osSendMesg(&Mutex[vol].mq, (OSMesg)0, OS_MESG_BLOCK);

#endif
}

#endif	/* FF_FS_REENTRANT */

