; Copyright 1999-2022 Aske Simon Christensen. See LICENSE.txt for usage terms.

WRITE		=	0
COMMANDLINE	=	0

	if	WRITE
	if	COMMANDLINE
	auto	wb MiniHeaderC.bin\MiniHeader\MiniHeader_End\
	else
	auto	wb MiniHeader.bin\MiniHeader\MiniHeader_End\
	endc
	endc

INIT_ONE_PROB		=	$8000
ADJUST_SHIFT		=	4
SINGLE_BIT_CONTEXTS	=	1
DUMMY_CONTEXT_OFFSET	=	0

LIB_VERSION	=	20
CacheClearU	=	-636


	align	0,4
MiniHeader:
	move.l	MiniHeader-4(pc),d2
	lsl.l	#2,d2
	move.l	d2,a1
	addq.l	#4,a1
	pea.l	(a1)
ContextOffsetInstr:
	lea.l	MiniHeader_End+DUMMY_CONTEXT_OFFSET+(32768/8)*2(pc),a2
	moveq.l	#0,d1
.init:	move.w	#INIT_ONE_PROB,-(a2)
	addq.w	#8,d1
	bpl.b	.init

	move.l	a2,a4
Depack:
	; A4 = Packed data End
	; A1 = Target
	; A2 = Contexts

	; D1 = $00008000
	; Lowest bit of D2 = 0
	swap.w	d1
	moveq.l	#1,d3
	moveq.l	#0,d6
.lit:
	addq.b	#1,d6
.getlit:
	bsr.b	GetBit
	addx.b	d6,d6
	bcc.b	.getlit
	move.b	d6,(a1)+
.switch:
	bsr.b	GetKind
	bcc.b	.lit
.ref:
	moveq.l	#-1,d6
	bsr.b	GetBit
	bcc.b	.readoffset
.readlength:
	moveq.l	#4,d6
	bsr.b	GetNumber
.copyloop:
	move.b	(a1,d5.l),(a1)+
	subq.l	#1,d7
	bne.b	.copyloop
.afterref:
	bsr.b	GetKind
	bcc.b	.lit
.readoffset:
	moveq.l	#3,d6
	bsr.b	GetNumber
	moveq.l	#2,d5
	sub.l	d7,d5
	bne.b	.readlength

	move.l	$4.w,a6
	cmp.w	#37,LIB_VERSION(a6)
	blt.b	not204
	if	COMMANDLINE
	movem.l	d0/a0,-(a7)
	jsr	CacheClearU(a6)
	movem.l	(a7)+,d0/a0
	; Fall through and eventually hit RTS
	else
	jmp	CacheClearU(a6)
	endc


GetKind:
	move.l	a1,d4
	moveq.l	#1,d6
	and.l	d4,d6
	lsl.w	#8,d6
	bra.b	GetBit

GetNumber:
	; D6 = Number context

	; Out: Number in D7
	lsl.w	#8,d6
.numberloop:
	addq.b	#2,d6
	bsr.b	GetBit
	bcs.b	.numberloop
	moveq.l	#1,d7
	subq.b	#1,d6
.bitsloop:
	bsr.b	GetBit
	addx.l	d7,d7
	subq.b	#2,d6
	bcc.b	.bitsloop
not204:	rts

	; D6 = Bit context

	; D1 = Input bit buffer
	; D2 = Range value
	; D3 = Interval size

	; Out: Bit in C and X

readbit:
	add.l	d1,d1
	bne.b	nonewword
	move.l	-(a4),d1
	addx.l	d1,d1
nonewword:
	addx.w	d2,d2
	add.w	d3,d3
GetBit:
	tst.w	d3
	bpl.b	readbit

	lea.l	SINGLE_BIT_CONTEXTS*2(a2,d6.l),a5
	add.l	d6,a5
	move.w	(a5),d4
	; D4 = One prob

	lsr.w	#ADJUST_SHIFT,d4
	sub.w	d4,(a5)
	add.w	(a5),d4

	mulu.w	d3,d4
	swap.w	d4

	sub.w	d4,d2
	blo.b	.one
.zero:
	; oneprob = oneprob * (1 - adjust) = oneprob - oneprob * adjust
	sub.w	d4,d3
	; 0 in C and X
	rts
.one:
	; onebrob = 1 - (1 - oneprob) * (1 - adjust) = oneprob - oneprob * adjust + adjust
	add.w	#$ffff>>ADJUST_SHIFT,(a5)
	move.w	d4,d3
	add.w	d4,d2
	; 1 in C and X
	rts

MiniHeader_InnerEnd:
	align	0,4
MiniHeader_End:

	printv	MiniHeader_InnerEnd-MiniHeader
	printt
	printv	ContextOffsetInstr+2-MiniHeader
