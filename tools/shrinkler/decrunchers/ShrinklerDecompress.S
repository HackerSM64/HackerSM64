; Copyright 1999-2022 Aske Simon Christensen.
;
; The code herein is free to use, in whole or in part,
; modified or as is, for any legal purpose.
;
; No warranties of any kind are given as to its behavior
; or suitability.


INIT_ONE_PROB		=	$8000
ADJUST_SHIFT		=	4
SINGLE_BIT_CONTEXTS	=	1
NUM_CONTEXTS		=	1536


; exec.library
EXEC_AllocMem		=	-198
EXEC_FreeMem		=	-210
EXEC_OldOpenLibrary	=	-408
EXEC_CloseLibrary	=	-414

MEMF_ANY		=	0
MEMF_CHIP		=	2
MEMF_FAST		=	4

; dos.library
DOS_Open		=	-30
DOS_Close		=	-36
DOS_Read		=	-42

MODE_OLDFILE		=	1005

; Data header
shr_id			=	0
shr_major		=	4
shr_minor		=	5
shr_header_size		=	6
shr_compressed_size	=	8
shr_uncompressed_size	=	12
shr_safety_margin	=	16
shr_flags		=	20

FLAG_PARITY_CONTEXT	=	1


; Decompress Shrinkler-compressed data produced with the --data option.
;
; A0 = Compressed data
; A1 = Decompressed data destination
; A2 = Progress callback, can be zero if no callback is desired.
;      Callback will be called continuously with
;      D0 = Number of bytes decompressed so far
;      A0 = Callback argument
;      D1 = Second callback argument
; A3 = Callback argument
; D2 = Second callback argument
; D7 = 0 to disable parity context (Shrinkler --bytes / -b option)
;      1 to enable parity context (default Shrinkler compression)
;
; Uses 3 kilobytes of space on the stack.
; Preserves D2-D7/A2-A6 and assumes callback does the same.
;
; Decompression code may read one byte beyond compressed data.
; The contents of this byte does not matter.

ShrinklerDecompress:
	movem.l	d2-d7/a4-a6,-(a7)

	move.l	a0,a4
	move.l	a1,a5
	move.l	a1,a6

	; Init range decoder state
	moveq.l	#0,d2
	moveq.l	#1,d3
	moveq.l	#-$80,d4

	; Init probabilities
	move.l	#NUM_CONTEXTS,d6
.init:	move.w	#INIT_ONE_PROB,-(a7)
	subq.w	#1,d6
	bne.b	.init

	; D6 = 0
.lit:
	; Literal
	addq.b	#1,d6
.getlit:
	bsr.b	GetBit
	addx.b	d6,d6
	bcc.b	.getlit
	move.b	d6,(a5)+
	bsr.b	ReportProgress
.switch:
	; After literal
	bsr.b	GetKind
	bcc.b	.lit
	; Reference
	moveq.l	#-1,d6
	bsr.b	GetBit
	bcc.b	.readoffset
.readlength:
	moveq.l	#4,d6
	bsr.b	GetNumber
.copyloop:
	move.b	(a5,d5.l),(a5)+
	subq.l	#1,d0
	bne.b	.copyloop
	bsr.b	ReportProgress
	; After reference
	bsr.b	GetKind
	bcc.b	.lit
.readoffset:
	moveq.l	#3,d6
	bsr.b	GetNumber
	moveq.l	#2,d5
	sub.l	d0,d5
	bne.b	.readlength

	lea.l	NUM_CONTEXTS*2(a7),a7
	movem.l	(a7)+,d2-d7/a4-a6
	rts

ReportProgress:
	move.l	a2,d0
	beq.b	.nocallback
	move.l	a5,d0
	sub.l	a6,d0
	move.l	a3,a0
	move.l	4+NUM_CONTEXTS*2(a7),d1
	jsr	(a2)
.nocallback:
	rts

GetKind:
	; Use parity as context
	move.l	a5,d6
	and.l	d7,d6
	lsl.w	#8,d6
	bra.b	GetBit

GetNumber:
	; D6 = Number context

	; Out: Number in D0
	lsl.w	#8,d6
.numberloop:
	addq.b	#2,d6
	bsr.b	GetBit
	bcs.b	.numberloop
	moveq.l	#1,d0
	subq.b	#1,d6
.bitsloop:
	bsr.b	GetBit
	addx.l	d0,d0
	subq.b	#2,d6
	bcc.b	.bitsloop
	rts

	; D6 = Bit context

	; D2 = Range value
	; D3 = Interval size
	; D4 = Input bit buffer

	; Out: Bit in C and X

readbit:
	add.b	d4,d4
	bne.b	nonewword
	move.b	(a4)+,d4
	addx.b	d4,d4
nonewword:
	addx.w	d2,d2
	add.w	d3,d3
GetBit:
	tst.w	d3
	bpl.b	readbit

	lea.l	4+SINGLE_BIT_CONTEXTS*2(a7,d6.l),a1
	add.l	d6,a1
	move.w	(a1),d1
	; D1 = One prob

	lsr.w	#ADJUST_SHIFT,d1
	sub.w	d1,(a1)
	add.w	(a1),d1

	mulu.w	d3,d1
	swap.w	d1

	sub.w	d1,d2
	blo.b	.one
.zero:
	; oneprob = oneprob * (1 - adjust) = oneprob - oneprob * adjust
	sub.w	d1,d3
	; 0 in C and X
	rts
.one:
	; onebrob = 1 - (1 - oneprob) * (1 - adjust) = oneprob - oneprob * adjust + adjust
	add.w	#$ffff>>ADJUST_SHIFT,(a1)
	move.w	d1,d3
	add.w	d1,d2
	; 1 in C and X
	rts


; Load and decompress a Shrinkler-compressed data file with header.
;
; A0 = Filename
; D4 = Memory attributes (i.e. MEMF_ANY, MEMF_CHIP, MEMF_FAST)
; D5 = Alignment (must be a power of two)
; A2 = Progress callback, can be zero if no callback is desired.
;      Callback will be called continuously with
;      D0 = Number of bytes decompressed so far
;      A0 = Callback argument
;      D1 = Uncompressed length
; A3 = Callback argument
;
; Out: D0 = Loaded and decompressed data, or 0 on error

ShrinklerLoad:
	movem.l	d2-d7/a4-a6,-(a7)
	move.l	a7,a4
	move.l	a0,d6
	clr.l	d7

	; Open DOS
	lea	.dosname(pc),a1
	move.l	$4.w,a6
	jsr	EXEC_OldOpenLibrary(a6)
	move.l	d0,a5

	; Open file
	move.l	d6,d1
	move.l	#MODE_OLDFILE,d2
	move.l	a5,a6
	jsr	DOS_Open(a6)
	move.l	d0,d6
	beq.w	.exit

	; Read ID and header length
	subq.l	#8,a7
	move.l	d6,d1
	move.l	a7,d2
	moveq.l	#8,d3
	jsr	DOS_Read(a6)
	cmp.l	d3,d0
	blt.b	.close

	; Check ID
	cmp.l	#"Shri",shr_id(a7)
	bne.b	.close

	; Load header
	clr.l	d3
	move.w	shr_header_size(a7),d3
	sub.l	d3,a7
	move.l	d6,d1
	move.l	a7,d2
	jsr	DOS_Read(a6)
	cmp.l	d3,d0
	blt.b	.close

	; Allocate memory
	move.l	d4,d1
	cmp.l	#8,d5
	bge.b	.big_alignment
	moveq.l	#8,d5
.big_alignment:
	move.l	shr_safety_margin-8(a7),d4
	bpl.b	.positive_margin
	moveq.l	#0,d4
.positive_margin:
	add.l	shr_uncompressed_size-8(a7),d4
	add.l	d5,d4
	move.l	d4,d0
	move.l	$4.w,a6
	jsr	EXEC_AllocMem(a6)
	move.l	d0,d1
	beq.b	.close

	; Align memory and write dealloc info
	add.l	d5,d1
	neg.l	d5
	and.l	d1,d5
	move.l	d5,a1
	move.l	d0,-(a1)
	move.l	d4,-(a1)

	; Load compressed data
	move.l	shr_compressed_size-8(a7),d3
	move.l	d6,d1
	move.l	d0,d2
	add.l	d4,d2
	sub.l	d3,d2
	move.l	a5,a6
	jsr	DOS_Read(a6)
	cmp.l	d3,d0
	blt.b	.close

	; Decompress
	move.l	d2,a0
	move.l	d5,a1
	moveq.l	#FLAG_PARITY_CONTEXT,d7
	and.l	shr_flags-8(a7),d7
	move.l	shr_uncompressed_size-8(a7),d2
	bsr.w	ShrinklerDecompress
	move.l	d5,d7

.close:
	move.l	d6,d1
	move.l	a5,a6
	jsr	DOS_Close(a6)

.exit:
	; Close DOS
	move.l	a5,a1
	move.l	$4.w,a6
	jsr	EXEC_CloseLibrary(a6)

	move.l	d7,d0
	move.l	a4,a7
	movem.l	(a7)+,d2-d7/a4-a6
	rts

.dosname:
	dc.b	"dos.library",0


; Free the memory for a file loaded with ShrinklerLoad.
;
; A0 = Loaded data

ShrinklerFree:
	move.l	a6,-(a7)
	move.l	-(a0),a1
	move.l	-(a0),d0
	move.l	$4.w,a6
	jsr	EXEC_FreeMem(a6)
	move.l	(a7)+,a6
	rts
