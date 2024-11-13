;
;  Size-optimized ApLib decompressor by spke & uniabis (ver.04 01-07/06/2020, 139 bytes)
;
;  The original Z80 decompressor for ApLib was written by Dan Weiss (Dwedit),
;  then tweaked by Francisco Javier Pena Pareja (utopian),
;  and optimized by Jaime Tejedor Gomez (Metalbrain).
;
;  This version was heavily re-optimized for size by spke.
;  (It is 17 bytes shorter and 22% faster than the 156b version by Metalbrain.)
;
;  ver.00 by spke (21/08/2018-01/09/2018, 141 bytes);
;  ver.01 by spke (spring 2019, 140(-1) bytes, slightly faster);
;  ver.02 by spke (05-07/01/2020, added full revision history, support for long offsets
;                  and an option to use self-modifying code instead of IY)
;  ver.03 by spke (18-29/05/2020, +0.5% speed, added support for backward compression)
;  ver.04 by uniabis (01-07/06/2020, 139(-1) bytes, +1% speed, added support for HD64180)
;
;  The data must be compressed using any compressor for ApLib capable of generating raw data.
;  At present, two best available compressors are:
;
;  "APC" by Sven-Ake Dahl: https://github.com/svendahl/cap or
;  "apultra" by Emmanuel Marty: https://github.com/emmanuel-marty/apultra
;
;  The compression can be done as follows:
;
;  apc.exe e <sourcefile> <outfile>
;  or
;  apultra.exe <sourcefile> <outfile>
;
;  A decent compressor was written by r57shell (although it is worse than compressors above):
;  http://gendev.spritesmind.net/forum/viewtopic.php?p=32548#p32548
;  The use of the official ApLib compressor by Joergen Ibsen is not recommended.
;
;  The decompression is done in the standard way:
;
;  ld hl,FirstByteOfCompressedData
;  ld de,FirstByteOfMemoryForDecompressedData
;  call DecompressApLib
;
;  Backward decompression is also supported; you can compress files backward using:
;
;  apultra.exe -b <sourcefile> <outfile>
;
;  uncomment option "DEFINE BackwardDecompression" and decompress the resulting files using:
;
;  ld hl,LastByteOfCompressedData
;  ld de,LastByteOfMemoryForDecompressedData
;  call DecompressApLib
;
;  The decompressor modifies AF, AF', BC, DE, HL, IX.
;
;  Of course, ApLib compression algorithms are (c) 1998-2014 Joergen Ibsen,
;  see http://www.ibsensoftware.com/ for more information
;
;  Drop me an email if you have any comments/ideas/suggestions: zxintrospec@gmail.com
;
;  This software is provided 'as-is', without any express or implied
;  warranty.  In no event will the authors be held liable for any damages
;  arising from the use of this software.
;
;  Permission is granted to anyone to use this software for any purpose,
;  including commercial applications, and to alter it and redistribute it
;  freely, subject to the following restrictions:
;
;  1. The origin of this software must not be misrepresented; you must not
;     claim that you wrote the original software. If you use this software
;     in a product, an acknowledgment in the product documentation would be
;     appreciated but is not required.
;  2. Altered source versions must be plainly marked as such, and must not be
;     misrepresented as being the original software.
;  3. This notice may not be removed or altered from any source distribution.

;	DEFINE FasterGetBit					; 16% speed-up at the cost of extra 4 bytes
;	DEFINE SupportLongOffsets				; +4 bytes for long offset support. slows decompression down by 1%, but may be needed to decompress files >=32K
;	DEFINE BackwardDecompression				; decompress data compressed backwards, -5 bytes, speeds decompression up by 3%


	IFDEF FasterGetBit
		MACRO	GET_BIT
			add a : call z,ReloadByte
		ENDM
	ELSE
		MACRO	GET_BIT
			call GetOneBit
		ENDM
	ENDIF

	IFNDEF BackwardDecompression

		MACRO NEXT_HL
		inc hl
		ENDM

		MACRO COPY_1
		ldi
		ENDM

		MACRO COPY_BC
		ldir
		ENDM

	ELSE

		MACRO NEXT_HL
		dec hl
		ENDM

		MACRO COPY_1
		ldd
		ENDM

		MACRO COPY_BC
		lddr
		ENDM

	ENDIF

@DecompressApLib:	ld a,128

;
;  case "0"+BYTE: copy a single literal

CASE0:			COPY_1					; first byte is always copied as literal
ResetLWM:		ld b,-1					; LWM = 0 (LWM stands for "Last Was Match"; a flag that we did not have a match)

;
;  main decompressor loop

MainLoop:		GET_BIT : jr nc,CASE0			; "0"+BYTE = copy literal
			GET_BIT : jr nc,CASE10			; "10"+gamma(offset/256)+BYTE+gamma(length) = the main matching mechanism

			ld bc,%11100000
			GET_BIT : jr nc,CASE110			; "110"+[oooooool] = matched 2-3 bytes with a small offset

;
;  case "111"+"oooo": copy a byte with offset -1..-15, or write zero to dest

CASE111:
ReadFourBits		GET_BIT					; read short offset (4 bits)
			rl c : jr c,ReadFourBits
			ex de,hl : jr z,WriteZero		; zero offset means "write zero" (NB: B is zero here)

			; "write a previous byte (1-15 away from dest)"
			push hl					; BC = offset, DE = src, HL = dest
	IFNDEF BackwardDecompression
			sbc hl,bc				; HL = dest-offset (SBC works because branching above ensured NC)
	ELSE
			add hl,bc				; HL = dest-offset (SBC works because branching above ensured NC)
	ENDIF
			ld c,(hl) : pop hl

WriteZero		ld (hl),c : NEXT_HL
			ex de,hl : jr ResetLWM			; write one byte, reset LWM

;
;  branch "110"+[oooooool]: copy two or three bytes (bit "l") with the offset -1..-127 (bits "ooooooo"), or stop

CASE110:		; "use 7 bit offset, length = 2 or 3"
			; "if a zero is found here, it's EOF"
			ld c,(hl) : rr c : ret z		; process EOF
			NEXT_HL

			push hl					; save src
			ld h,b : ld l,c				; HL = offset

			; flag NC means len=2, flag C means len=3
			ld c,1 : rl c : jr SaveLWMOffset
			
;
;  branch "10"+gamma(offset/256)+BYTE+gamma(length): the main matching mechanism

CASE10:			; save state of LWM into A'
			exa : ld a,b : exa

			; "use a gamma code * 256 for offset, another gamma code for length"
			call GetGammaCoded

			; the original decompressor contains
			;
			; if ((LWM == 0) && (offs == 2)) { ... }
			; else {
			;	if (LWM == 0) { offs -= 3; }
			;	else { offs -= 2; }
			; }
			;
			; so, the idea here is to use the fact that GetGammaCoded returns (offset/256)+2,
			; and to split the first condition by noticing that C-1 can never be zero
			exa : add c : ld c,a : exa

			; "if gamma code is 2, use old r0 offset"
			dec c : jr z,KickInLWM
			dec c
			ld b,c : ld c,(hl) : NEXT_HL		; BC = offset

			push bc					; (SP) = offset
			call GetGammaCoded			; BC = len*
			ex (sp),hl				; HL = offset, (SP) = src

			; interpretation of length value is offset-dependent
			exa : ld a,h
	IFDEF	SupportLongOffsets
			; NB offsets over 32000 require an additional check, which is skipped in most
			; Z80 decompressors (seemingly as a performance optimization)
			cp 32000/256 : jr nc,.Add2
	ENDIF
			cp 5 : jr nc,.Add1
			or a : jr nz,.Add0
			bit 7,l : jr nz,.Add0
.Add2			inc bc
.Add1			inc bc
.Add0			exa

SaveLWMOffset:
			push hl : pop ix			; save offset for future LWMs

CopyMatch:		; this assumes that BC = len, DE = dest, HL = offset
			; and also that (SP) = src, while having NC
	IFNDEF BackwardDecompression
			push de
			ex de,hl : sbc hl,de			; HL = dest-offset
			pop de					; DE = dest
	ELSE
			add hl,de				; HL = dest+offset
	ENDIF

			COPY_BC
			pop hl					; recover src
			jr MainLoop

;
;  the re-use of the previous offset (LWM magic)

KickInLWM:		; "and a new gamma code for length"
			call GetGammaCoded			; BC = len
			push ix : ex (sp),hl			; DE = dest, HL = prev offset
			jr CopyMatch

;
;  interlaced gamma code reader
;  x0 -> 1x
;  x1y0 -> 1xy
;  x1y1z0 -> 1xyz etc
;  (technically, this is a 2-based variation of Exp-Golomb-1)

GetGammaCoded:		ld bc,1
ReadGamma		GET_BIT : rl c : rl b
			GET_BIT : ret nc
			jr ReadGamma

;
;  pretty usual getbit for mixed datastreams

	IFNDEF FasterGetBit
GetOneBit:		add a : ret nz
	ENDIF
ReloadByte:		ld a,(hl) : NEXT_HL
			rla : ret

