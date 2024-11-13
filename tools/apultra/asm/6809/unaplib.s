;  unaplib.s - aPLib decompressor for 6809 - 157 bytes
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

apl_decompress
         lda #$80          ; initialize empty bit queue
         sta <apbitbuf,pcr ; plus bit to roll into carry
         leau ,x

apcplit  ldb ,u+           ; copy literal byte
apwtlit  stb ,y+

         lda #$03          ; set 'follows literal' flag

aptoken  bsr apgetbit      ; read 'literal or match' bit
         bcc apcplit       ; if 0: literal

         bsr apgetbit      ; read '8+n bits or other type' bit
         bcs apother       ; if 11x: other type of match

         sta <aplwm+2,pcr  ; store 'follows literal' flag

         bsr apgamma2      ; 10: read gamma2-coded high offset bits
aplwm    subd #$0000       ; high offset bits == 2 when follows_literal == 3 ?
         bcc apnorep       ; if not, not a rep-match

         bsr apgamma2      ; read repmatch length
         bra apgotlen      ; go copy large match

apnorep  tfr b,a           ; transfer high offset bits to A
         ldb ,u+           ; read low offset byte in B
         std <aprepof+1,pcr ; store match offset
         tfr d,x           ; transfer offset to X

         bsr apgamma2      ; read match length

         cmpx #$7D00       ; offset >= 32000 ?
         bge apincby2      ; if so, increase match len by 2
         cmpx #$0500       ; offset >= 1280 ?
         bge apincby1      ; if so, increase match len by 1
         cmpx #$80         ; offset < 128 ?
         bge apgotlen      ; if so, increase match len by 2
apincby2 addd #1
apincby1 addd #1
apgotlen pshs u            ; save source compressed data pointer
         tfr d,x           ; copy match length to X

aprepof  ldd #$aaaa        ; load match offset
         nega              ; reverse sign of offset in D
         negb
         sbca #0
         leau d,y          ; put backreference start address in U (dst+offset)

apcpymt  lda ,u+           ; copy matched byte
         sta ,y+
         leax -1,x         ; decrement X
         bne apcpymt       ; loop until all matched bytes are copied

         puls u            ; restore source compressed data pointer

         lda #$02          ; clear 'follows literal' flag
         bra aptoken

apdibits bsr apgetbit      ; read bit
         rolb              ; push into B
apgetbit lsl <apbitbuf,pcr ; shift bit queue, and high bit into carry
         bne apdone        ; queue not empty, bits remain
         pshs a            ; save reg A
         lda ,u+           ; read 8 new bits
         rola              ; shift bit queue, and high bit into carry
         sta <apbitbuf,pcr ; save bit queue
         puls a,pc         ; pop reg A and return

apbitbuf fcb $00           ; bit queue

apshort  clrb
         bsr apdibits      ; read 2 offset bits
         rolb
         bsr apdibits      ; read 4 offset bits
         rolb
         beq apwtlit       ; go write zero

         negb              ; reverse offset in D
         ldb b,y           ; load backreferenced byte from dst+offset
         bra apwtlit       ; go write backreferenced byte

apgamma2 ldd #$1           ; init to 1 so it gets shifted to 2 below
apg2loop bsr apgetbit      ; read data bit
         rolb              ; shift into D
         rola
         bsr apgetbit      ; read continuation bit
         bcs apg2loop      ; loop until a zero continuation bit is read
apdone   rts

apother  bsr apgetbit      ; read '7+1 match or short literal' bit
         bcs apshort       ; if 111: 4 bit offset for 1-byte copy

         ldb ,u+           ; read low bits of offset + length bit in B
         beq apdone        ; check for EOD
         clra              ; clear high bits in A
         lsrb              ; shift offset in place, shift length bit into carry
         std <aprepof+1,pcr ; store match offset
         ldb #$01          ; len in B will be 2*1+carry:
         rolb              ; shift length, and carry into B
         bra apgotlen      ; go copy match
