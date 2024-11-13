#ifndef DOS_DOSHUNKS_H
#define DOS_DOSHUNKS_H
/*
**	$VER: doshunks.h 36.9 (2.6.92)
**	Includes Release 40.13
**
**	Hunk definitions for object and load modules.
**
**	(C) Copyright 1989-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/* hunk types */
#define HUNK_UNIT	999
#define HUNK_NAME	1000
#define HUNK_CODE	1001
#define HUNK_DATA	1002
#define HUNK_BSS	1003
#define HUNK_RELOC32	1004
#define HUNK_ABSRELOC32	HUNK_RELOC32
#define HUNK_RELOC16	1005
#define HUNK_RELRELOC16	HUNK_RELOC16
#define HUNK_RELOC8	1006
#define HUNK_RELRELOC8	HUNK_RELOC8
#define HUNK_EXT	1007
#define HUNK_SYMBOL	1008
#define HUNK_DEBUG	1009
#define HUNK_END	1010
#define HUNK_HEADER	1011

#define HUNK_OVERLAY	1013
#define HUNK_BREAK	1014

#define HUNK_DREL32	1015
#define HUNK_DREL16	1016
#define HUNK_DREL8	1017

#define HUNK_LIB	1018
#define HUNK_INDEX	1019

/*
 * Note: V37 LoadSeg uses 1015 (HUNK_DREL32) by mistake.  This will continue
 * to be supported in future versions, since HUNK_DREL32 is illegal in load files
 * anyways.  Future versions will support both 1015 and 1020, though anything
 * that should be usable under V37 should use 1015.
 */
#define HUNK_RELOC32SHORT 1020

/* see ext_xxx below.  New for V39 (note that LoadSeg only handles RELRELOC32).*/
#define HUNK_RELRELOC32	1021
#define HUNK_ABSRELOC16	1022

/*
 * Any hunks that have the HUNKB_ADVISORY bit set will be ignored if they
 * aren't understood.  When ignored, they're treated like HUNK_DEBUG hunks.
 * NOTE: this handling of HUNKB_ADVISORY started as of V39 dos.library!  If
 * lading such executables is attempted under <V39 dos, it will fail with a
 * bad hunk type.
 */
#define HUNKB_ADVISORY	29
#define HUNKB_CHIP	30
#define HUNKB_FAST	31
#define HUNKF_ADVISORY	(1U<<29)
#define HUNKF_CHIP	(1U<<30)
#define HUNKF_FAST	(1U<<31)


/* hunk_ext sub-types */
#define EXT_SYMB	0	/* symbol table */
#define EXT_DEF		1	/* relocatable definition */
#define EXT_ABS		2	/* Absolute definition */
#define EXT_RES		3	/* no longer supported */
#define EXT_REF32	129	/* 32 bit absolute reference to symbol */
#define EXT_ABSREF32	EXT_REF32
#define EXT_COMMON	130	/* 32 bit absolute reference to COMMON block */
#define EXT_ABSCOMMON	EXT_COMMON
#define EXT_REF16	131	/* 16 bit PC-relative reference to symbol */
#define EXT_RELREF16	EXT_REF16
#define EXT_REF8	132	/*  8 bit PC-relative reference to symbol */
#define EXT_RELREF8	EXT_REF8
#define EXT_DEXT32	133	/* 32 bit data relative reference */
#define EXT_DEXT16	134	/* 16 bit data relative reference */
#define EXT_DEXT8	135	/*  8 bit data relative reference */

/* These are to support some of the '020 and up modes that are rarely used */
#define EXT_RELREF32	136	/* 32 bit PC-relative reference to symbol */
#define EXT_RELCOMMON	137	/* 32 bit PC-relative reference to COMMON block */

/* for completeness... All 680x0's support this */
#define EXT_ABSREF16	138	/* 16 bit absolute reference to symbol */

/* this only exists on '020's and above, in the (d8,An,Xn) address mode */
#define EXT_ABSREF8	139	/* 8 bit absolute reference to symbol */

#endif	/* DOS_DOSHUNKS_H */
