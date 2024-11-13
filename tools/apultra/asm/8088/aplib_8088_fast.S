;  aplib_8088_fast.S - speed-optimized aPLib decompressor for 8088 - 188 bytes
;
;  Copyright (C) 2019 Emmanuel Marty
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

        segment .text
        bits 16

;  ---------------------------------------------------------------------------
;  Decompress aPLib data
;  inputs:
;  * ds:si: compressed aPLib data
;  * es:di: output buffer
;  output:
;  * ax:    decompressed size
;  ---------------------------------------------------------------------------

%macro apl_get_bit 0            ; read bit into carry
        add     al,al           ; shift bit queue, and high bit into carry
        jnz     %%gotbit        ; queue not empty, bits remain
        lodsb                   ; read 8 new bits
        adc     al,al           ; shift bit queue, and high bit into carry
%%gotbit:
%endmacro

apl_decompress:
        push    di              ; remember decompression offset
        cld                     ; make string operations go forward

        ; === register map ===
        ; al: bit queue
        ; ah: unused, but value is trashed
        ; bx: follows_literal
        ; cx: scratch register for reading gamma2 codes and storing copy length
        ; dx: match offset (and rep-offset)
        ; si: input (compressed data) pointer
        ; di: output (decompressed data) pointer
        ; bp: temporary value, trashed

        mov     al,080H         ; clear bit queue(al) and set high bit to move into carry
        xor     dx,dx           ; invalidate rep offset

.literal:
        movsb                   ; read and write literal byte
.next_command_after_literal:
        mov     bx,03H          ; set follows_literal(bx) to 3

.next_command:
        apl_get_bit             ; read 'literal or match' bit
        jnc     .literal        ; if 0: literal
                                
                                ; 1x: match

        apl_get_bit             ; read '8+n bits or other type' bit
        jc      .other          ; 11x: other type of match

                                ; 10: 8+n bits match
        call    .get_gamma2     ; read gamma2-coded high offset bits
        sub     cx,bx           ; high offset bits == 2 when follows_literal == 3 ?
                                ; (a gamma2 value is always >= 2, so substracting follows_literal when it
                                ; is == 2 will never result in zero)
        jae     .not_repmatch   ; if not, not a rep-match

        call    .get_gamma2     ; read match length
        jmp     short .got_len  ; go copy

.not_repmatch:
        mov     dh,cl           ; transfer high offset bits to dh
        mov     dl,[si]         ; read low offset byte in dl
        inc     si

        call    .get_gamma2     ; read match length
        cmp     dh,07DH         ; offset >= 32000 ?
        jae     .increase_len_by2 ; if so, increase match len by 2
        cmp     dh,05H          ; offset >= 1280 ?
        jae     .increase_len_by1 ; if so, increase match len by 1
        cmp     dx,0080H        ; offset < 128 ?
        jae     .got_len        ; if so, increase match len by 2, otherwise it would be a 7+1 copy
.increase_len_by2:
        inc     cx              ; increase length
.increase_len_by1:
        inc     cx              ; increase length

        ; copy cx bytes from match offset dx

.got_len:
        push    ds              ; save ds:si (current pointer to compressed data)
        mov     bp,si

        push    es
        pop     ds
        mov     si,di           ; point to destination in es:di - offset in dx
        sub     si,dx
        rep     movsb           ; copy matched bytes

        mov     si,bp           ; restore ds:si
        pop     ds

        mov     bl,02H          ; set follows_literal to 2 (bx is unmodified by match commands)
        jmp     short .next_command

        ; read gamma2-coded value into cx

.get_gamma2:
        xor     cx,cx           ; initialize to 1 so that value will start at 2
        inc     cx              ; when shifted left in the adc below

.gamma2_loop:
        apl_get_bit             ; read data bit
        adc     cx,cx           ; shift into cx
        apl_get_bit             ; read continuation bit
        jc      .gamma2_loop    ; loop until a zero continuation bit is read

        ret

        ; handle 7 bits offset + 1 bit len or 4 bits offset / 1 byte copy

.other:
        xor     cx,cx
        apl_get_bit             ; read '7+1 match or short literal' bit
        jc      .short_literal  ; 111: 4 bit offset for 1-byte copy

                                ; 110: 7 bits offset + 1 bit length
                                
        mov     dl,[si]         ; read offset + length in dl
        inc     si

        inc     cx              ; prepare cx for length below
        shr     dl,1            ; shift len bit into carry, and offset in place
        je      .done           ; if zero offset: EOD
        adc     cx,cx           ; len in cx: 1*2 + carry bit = 2 or 3

        xor     dh,dh           ; clear high bits of offset
        jmp     short .got_len

        ; 4 bits offset / 1 byte copy

.short_literal:
        apl_get_bit             ; read 4 offset bits
        adc     cl,cl
        apl_get_bit
        adc     cl,cl
        apl_get_bit
        adc     cl,cl
        apl_get_bit
        adc     cl,cl
        xchg    ax,cx           ; preserve bit queue in cx, put offset in ax
        jz      .write_zero     ; if offset is 0, write a zero byte

                                ; short offset 1-15
        mov     bx,di           ; point to destination in es:di - offset in ax
        sub     bx,ax           ; we trash bx, it will be reset to 3 when we loop
        mov     al,[es:bx]      ; read byte from short offset
.write_zero:
        stosb                   ; copy matched byte
        mov     ax,cx           ; restore bit queue in al
        jmp     .next_command_after_literal

.done:
        pop     ax              ; retrieve the original decompression offset
        xchg    di,ax           ; compute decompressed size
        sub     ax,di
        ret
