; -----------------------------------------------------------------------------
; aplib_6502_b.s - fast aPLib backward decompressor for 6502 - 253 bytes
; written for the ACME assembler
;
; jsr apl_decompress to unpack data backwards.
; create backwards compressed data with apultra -b or oapack -b
;
; in:
; * apl_srcptr (low and high byte) = last byte of compressed data
; * apl_dstptr (low and high byte) = last byte of decompression buffer
;
; out:
; * apl_dstptr (low and high byte) = first byte of decompressed data
;
;  Copyright (C) 2020 Emmanuel Marty
;  With parts of the code inspired by John Brandwood, Peter Ferrie
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
; -----------------------------------------------------------------------------

                  ; Zero page locations

apl_gamma2_hi     = $F6
apl_bitbuf        = $F7
apl_offset        = $F8
apl_winptr        = $FA
apl_srcptr        = $FC
apl_dstptr        = $FE

                  ; Read a byte from the source into A. Trashes X

                  !macro   APL_GET_SRC {
                  lda (apl_srcptr),y
                  ldx <apl_srcptr+0
                  bne .src_page_done
                  dec <apl_srcptr+1
.src_page_done:   dec <apl_srcptr+0
                  }

                  ; Write a byte to the destinatipn

                  !macro   APL_PUT_DST {
                  sta (apl_dstptr),y
                  lda <apl_dstptr+0
                  bne .dst_page_done
                  dec <apl_dstptr+1
.dst_page_done:   dec <apl_dstptr+0
                  }

                  ; Read one bit from the source into the carry, trash A

                  !macro   APL_GET_BIT {
                  asl <apl_bitbuf
                  bne .has_bits
                  jsr apl_load_bits
.has_bits:
                  }

                  ; Read one bit from the source into the carry, preserve A

                  !macro   APL_GET_BIT_SAVEA {
                  asl <apl_bitbuf
                  bne .has_bits
                  pha
                  jsr apl_load_bits
                  pla
.has_bits:
                  }

                  ; Decompress aPLib data backwards

apl_decompress:   lda #$80                      ; initialize empty bit queue
                  sta <apl_bitbuf               ; plus bit to roll into carry
                  ldy #$00                      ; clear Y for indirect addr

.copy_literal:    +APL_GET_SRC                  ; read literal from source
.write_literal:   +APL_PUT_DST                  ; write literal to destination

                  ldx #$00                      ; clear 'follows match' flag

.next_token:      +APL_GET_BIT                  ; read 'literal or match' bit
                  bcc .copy_literal             ; if 0: literal

                  +APL_GET_BIT                  ; read '8+n bits or other' bit
                  bcc .long_match               ; if 10x: long 8+n bits match
                                                
                                                ; 11x: other type of match

                  +APL_GET_BIT                  ; read '7+1 match or short literal' bit
                  bcs .short_match              ; if 111: 4 bit offset for 1-byte copy

                  +APL_GET_SRC                  ; read low byte of offset + length bit
                  lsr                           ; shift offset into place, len bit into carry
                  beq .done                     ; check for EOD
                  sta <apl_offset+0             ; store low byte of offset
                  sty <apl_offset+1             ; set high byte of offset to 0

                  tya                           ; set A to 0
                  sty <apl_gamma2_hi            ; set high byte of len to 0
                  adc #$02                      ; add 2 or 3 depending on len bit in carry
                                                ; now, low part of len is in A
                                                ; high part of len in apl_gamma2_hi is 0
                                                ; offset is written to apl_offset
                  bne .got_len                  ; go copy matched bytes

.long_match:      jsr .get_gamma2               ; 10: read gamma2 high offset bits in A
                  sty <apl_gamma2_hi            ; zero out high byte of gamma2

                  cpx #$01                      ; set carry if following literal
                  sbc #$02                      ; substract 3 if following literal, 2 otherwise
                  bcs .no_repmatch

                  jsr .get_gamma2               ; read repmatch length: low part in A
                  bcc .got_len                  ; go copy large match
                                                ; (carry is always clear after .get_gamma2)

.short_match:     lda #$10                      ; clear offset, load end bit into place
.read_short_offs: +APL_GET_BIT_SAVEA            ; read one bit of offset into carry
                  rol                           ; shift into A, shift end bit as well
                  bcc .read_short_offs          ; loop until end bit is shifted out into carry

                  beq .write_literal            ; zero offset means write a 0
                  tay
                  lda (apl_dstptr),y            ; load backreferenced byte
                  ldy #$00                      ; clear Y again
                  beq .write_literal            ; go write byte to destination

.get_gamma2:      lda #$01                      ; 1 so it gets shifted to 2
.gamma2_loop:     +APL_GET_BIT_SAVEA            ; read data bit
                  rol                           ; shift into low byte
                  rol <apl_gamma2_hi            ; shift into high byte
                  +APL_GET_BIT_SAVEA            ; read continuation bit
                  bcs .gamma2_loop              ; loop until a zero continuation bit is read
.done:            rts

.no_repmatch:     sta <apl_offset+1             ; write high byte of offset
                  +APL_GET_SRC                  ; read low byte of offset from source
                  sta <apl_offset+0             ; store low byte of offset

                  jsr .get_gamma2               ; read match length: low part in A

                  ldx <apl_offset+1             ; high offset byte is zero?
                  beq .offset_1byte             ; if so, offset < 256

                                                ; offset is >= 256.

                  cpx #$7d                      ; offset >= 32000 (7d00) ?
                  bcs .offset_incby2            ; if so, increase match len by 2
                  cpx #$05                      ; offset >= 1280 (0500) ?
                  bcs .offset_incby1            ; if so, increase match len by 1
                  bcc .got_len                  ; length is fine, go copy

.offset_1byte:    ldx <apl_offset+0             ; offset < 128 ?
                  bmi .got_len                  ; if so, increase match len by 2
                  sec                           ; carry must be set below

.offset_incby2:   adc #$01                      ; add 1 + set carry (from bcs or sec)
                  bcs .len_inchi                ; go add 256 to len if overflow

                                                ; carry clear: fall through for no-op

.offset_incby1:   adc #$00                      ; add 1 + carry
                  bcc .got_len
.len_inchi:       inc <apl_gamma2_hi            ; add 256 to len if low byte overflows

.got_len:         tax                           ; transfer low byte of len into X
                  beq .add_offset
                  inc <apl_gamma2_hi

.add_offset:      clc                           ; add dest + match offset
                  lda <apl_dstptr+0             ; low 8 bits
                  adc <apl_offset+0
                  sta <apl_winptr+0             ; store back reference address
                  lda <apl_dstptr+1             ; high 8 bits
                  adc <apl_offset+1
                  sta <apl_winptr+1             ; store high 8 bits of address

.copy_match_loop: lda (apl_winptr),y            ; read one byte of backreference
                  +APL_PUT_DST                  ; write byte to destination

                  lda <apl_winptr+0             ; decrement backreference address
                  bne .backref_page_done
                  dec <apl_winptr+1
.backref_page_done:
                  dec <apl_winptr+0

                  dex                           ; loop to copy all matched bytes
                  bne .copy_match_loop
                  dec <apl_gamma2_hi
                  bne .copy_match_loop

                                                ; X is 0 when exiting the loop above
                  inx                           ; set 'follows match' flag
                  jmp .next_token               ; go decode next token

apl_load_bits:    lda (apl_srcptr),y            ; read 8 bits from source
                  rol                           ; shift bit queue, and high bit into carry
                  sta <apl_bitbuf               ; save bit queue
                  
                  lda <apl_srcptr+0
                  bne .bits_page_done
                  dec <apl_srcptr+1
.bits_page_done:  dec <apl_srcptr+0
                  rts
