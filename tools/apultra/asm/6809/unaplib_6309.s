;  unaplib_6309.s - aPLib decompressor for H6309 - 131 bytes
;
;  in:  x = start of compressed data
;       y = start of decompression buffer
;  out: y = end of decompression buffer + 1
;
;  Copyright (C) 2020 Emmanuel Marty
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


; Original M6809 version written by Emmanuel Marty with Hitachi 6309 enhancements
; added by Doug Masten.
;
; Main advantage of H6309 CPU is the "TFM" instruction which can copy one
; byte of memory in 3 clock cycles vs a traditional copy loop that takes
; 20 clock cycles.

; Options:
;   APLIB_VAR
;     Define variable to point to a DP memory location for a memory space
;     and speed optimization.
;     ex. APLIB_VAR equ <memory location>
;
;   APLIB_LONG_OFFSET_DISABLE
;     Defined variable to disable long offsets >= 32000 for a speed and space
;     optimization. Only enable this if you know what you are doing.
;     ex. APLIB_LONG_OFFSET_DISABLE equ 1


; define options
         ifdef APLIB_VAR
apbitbuf equ APLIB_VAR     ; bit queue (use DP memory for mem & space optimization)
         else
apbitbuf fcb 0             ; bit queue (DEFAULT - use extended memory)
         endc


apl_decompress
         lda #$80          ; initialize empty bit queue
         sta apbitbuf      ; plus bit to roll into carry
         tfr x,u

apcplit  ldb ,u+           ; copy literal byte
apwtlit  stb ,y+

         ldb #3            ; set 'follows literal' flag

aptoken  bsr apgetbit      ; read 'literal or match' bit
         bcc apcplit       ; if 0: literal

         bsr apgetbit      ; read '8+n bits or other type' bit
         bcs apother       ; if 11x: other type of match

         bsr apgamma2      ; 10: read gamma2-coded high offset bits
         clra
         subr d,w          ; high offset bits == 2 when follows_literal == 3 ?
         bcc apnorep       ; if not, not a rep-match

         bsr apgamma2      ; read repmatch length
         bra apgotlen      ; go copy large match

apnorep  tfr f,a           ; transfer high offset bits to A
         ldb ,u+           ; read low offset byte in B
         tfr d,x           ; save match offset

         bsr apgamma2      ; read match length

         ifndef APLIB_LONG_OFFSET_DISABLE
         cmpx #$7D00       ; offset >= 32000 ?
         bge apincby2      ; if so, increase match len by 2
         endc
         cmpx #$0500       ; offset >= 1280 ?
         bge apincby1      ; if so, increase match len by 1
         cmpx #$80         ; offset < 128 ?
         bge apgotlen      ; if so, increase match len by 2
apincby2 incw
apincby1 incw

apgotlen tfr y,d           ; transfer dst to D
         subr x,d          ; put backreference start address in D (dst + offset)
         tfm d+,y+         ; copy matched bytes

         ldb #2            ; clear 'follows literal' flag
         bra aptoken

apgamma2 ldw #1            ; init to 1 so it gets shifted to 2 below
loop@    bsr apgetbit      ; read data bit
         rolw              ; shift into W
         bsr apgetbit      ; read continuation bit
         bcs loop@         ; loop until a zero continuation bit is read
         rts

apdibits bsr apgetbit      ; read bit
         rolb              ; push into B
apgetbit lsl apbitbuf      ; shift bit queue, and high bit into carry
         bne aprts         ; queue not empty, bits remain
         lda ,u+           ; read 8 new bits
         rola              ; shift bit queue, and high bit into carry
         sta apbitbuf      ; save bit queue
aprts    rts

apshort  clrb
         bsr apdibits      ; read 2 offset bits
         rolb
         bsr apdibits      ; read 4 offset bits
         rolb
         beq apwtlit       ; if zero, go write it

         negb              ; reverse offset in D
         ldb b,y           ; load backreferenced byte from dst+offset
         bra apwtlit       ; go write it

apother  bsr apgetbit      ; read '7+1 match or short literal' bit
         bcs apshort       ; if 111: 4 bit offset for 1-byte copy

         ldb ,u+           ; read low bits of offset + length bit in B
         beq aprts         ; check for EOD and exit if so
         clra              ; clear high bits in A
         lsrb              ; shift offset in place, shift length bit into carry
         tfr d,x           ; save match offset
         ldb #1            ; len in B will be 2*1+carry:
         rolb              ; shift length, and carry into B
         tfr d,w
         bra apgotlen      ; go copy match
