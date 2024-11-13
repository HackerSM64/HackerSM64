@APlib ARM7 decompressor by Dan Weiss, based on the original C version
@Takes in raw apacked data, NOT data created by the 'safe' compressor.
@Code is from the PocketNES NES Emulator for GBA

@Code is formatted for GNU Assembler

 src .req r0
 dest .req r1
 byte .req r2
 mask .req r3
 gamma .req r4
 lwm .req r6
 recentoff .req r7
 temp .req r8

.global depack
.type   depack STT_FUNC

@r0 = src
@r1 = dest
@r2 = byte
@r3 = rotating bit mask
@r4 = increasing gamma
@r6 = lwm
@r7 = recentoff
@r8 = lr copy/scratch

	.macro GETBIT @3 instructions
	movs mask,mask,ror #1
	ldrcsb byte,[src],#1
	tst byte,mask
	.endm

	.macro GETBITGAMMA @5 instructions
	mov gamma,gamma,lsl #1
	GETBIT
	addne gamma,gamma,#1
	.endm

@This initilaiztion code can go into slow memory

depack:
	stmfd sp!,{r4-r10,lr}
	ldrb temp,[src],#1
	strb temp,[dest],#1
	ldr mask,=0x01010101
	b aploop_nolwm

@This inner-loop code should be placed into fast memory

	@depack enters here
aploop_nolwm:
	mov lwm,#0
aploop:
	GETBIT
	bne apbranch1
	ldrb temp,[src],#1
	strb temp,[dest],#1
	b aploop_nolwm
apbranch1:
	GETBIT
	beq apbranch2
	GETBIT
	beq apbranch3
	@get an offset
	mov gamma,#0
	GETBIT
	addne gamma,gamma,#1
	GETBITGAMMA
	GETBITGAMMA
	GETBITGAMMA
	cmp gamma,#0
	ldrneb gamma,[dest,-gamma]
	strb gamma,[dest],#1
	b aploop_nolwm
apbranch3:
	@use 7 bit offset, length = 2 or 3
	@if a zero is encountered here, it's EOF
	ldrb gamma,[src],#1
	movs recentoff,gamma,lsr #1
	beq done
	ldrcsb temp,[dest,-recentoff]
	strcsb temp,[dest],#1
	ldrb temp,[dest,-recentoff]
	strb temp,[dest],#1
	ldrb temp,[dest,-recentoff]
	strb temp,[dest],#1
	mov lwm,#1
	b aploop
apbranch2:
	@use a gamma code * 256 for offset, another gamma code for length

	bl ap_getgamma
	sub gamma,gamma,#2
	cmp lwm,#0
	bne ap_is_lwm
	mov lwm,#1
	cmp gamma,#0
	bne ap_not_zero_gamma

	@if gamma code is 2, use old recent offset, and a new gamma code for length
	bl ap_getgamma
copyloop1:
	ldrb temp,[dest,-recentoff]
	strb temp,[dest],#1
	subs gamma,gamma,#1
	bne copyloop1
	b aploop
	
ap_not_zero_gamma:
	sub gamma,gamma,#1
ap_is_lwm:
	ldrb temp,[src],#1
	add recentoff,temp,gamma,lsl #8
	bl ap_getgamma
	@gamma=length
	cmp recentoff,#32000
	addge gamma,gamma,#1
	cmp recentoff,#1280
	addge gamma,gamma,#1
	cmp recentoff,#128
	addlt gamma,gamma,#2
copyloop2:
	ldrb temp,[dest,-recentoff]
	strb temp,[dest],#1
	subs gamma,gamma,#1
	bne copyloop2
	b aploop

ap_getgamma:
	mov gamma,#1
ap_getgammaloop:
	GETBITGAMMA
	GETBIT
	bne ap_getgammaloop
	bx lr

done:
	ldmfd sp!,{r4-r10,lr}
	bx lr

.unreq src
.unreq dest
.unreq byte
.unreq mask
.unreq gamma
.unreq lwm
.unreq recentoff
.unreq temp

