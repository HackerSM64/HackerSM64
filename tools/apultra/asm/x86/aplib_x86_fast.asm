;  aplib_x86_fast.asm - speed-optimized aPLib decompressor for x86 - 188 bytes
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
        bits 32

;  ---------------------------------------------------------------------------
;  Decompress aPLib data
;  inputs:
;  * esi: compressed aPLib data
;  * edi: output buffer
;  output:
;  * eax:    decompressed size
;  ---------------------------------------------------------------------------
        %ifndef BIN
          global apl_decompress
          global _apl_decompress
        %endif
        
        ; uint32_t apl_decompress(const void *Source, void *Destination);
        
%macro apl_get_bit 0            ; read bit into carry
        add     al,al           ; shift bit queue, and high bit into carry
        jnz     %%gotbit        ; queue not empty, bits remain
        lodsb                   ; read 8 new bits
        adc     al,al           ; shift bit queue, and high bit into carry
%%gotbit:
%endmacro

apl_decompress:
_apl_decompress:
        pushad
        
        %ifdef CDECL
          mov    esi, [esp+32+4]  ; esi = aPLib compressed data
          mov    edi, [esp+32+8]  ; edi = output
        %endif
        
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
        xor     edx, edx        ; invalidate rep offset
.literal:
        movsb                   ; read and write literal byte
.next_command_after_literal:
        mov     ebx,03H         ; set follows_literal(bx) to 3

.next_command:
        apl_get_bit             ; read 'literal or match' bit
        jnc     .literal        ; if 0: literal
                                
                                ; 1x: match

        apl_get_bit             ; read '8+n bits or other type' bit
        jc      .other          ; 11x: other type of match

                                ; 10: 8+n bits match
        call    .get_gamma2     ; read gamma2-coded high offset bits
        sub     ecx,ebx         ; high offset bits == 2 when follows_literal == 3 ?
                                ; (a gamma2 value is always >= 2, so substracting follows_literal when it
                                ; is == 2 will never result in zero)
        jae     .not_repmatch   ; if not, not a rep-match

        call    .get_gamma2     ; read match length
        jmp     .got_len        ; go copy

.not_repmatch:
        mov     edx,ecx         ; transfer high offset bits to dh
        shl     edx, 8
        mov     dl,[esi]        ; read low offset byte in dl
        inc     esi

        call    .get_gamma2     ; read match length
        cmp     edx,07D00H        ; offset >= 32000 ?
        jae     .increase_len_by2 ; if so, increase match len by 2
        cmp     edx,0500H         ; offset >= 1280 ?
        jae     .increase_len_by1 ; if so, increase match len by 1
        cmp     edx,0080H       ; offset < 128 ?
        jae     .got_len        ; if so, increase match len by 2, otherwise it would be a 7+1 copy
.increase_len_by2:
        inc     ecx             ; increase length
.increase_len_by1:
        inc     ecx             ; increase length

        ; copy cx bytes from match offset dx

.got_len:
        push    esi
        mov     esi,edi         ; point to destination in es:di - offset in dx
        sub     esi,edx
        rep     movsb           ; copy matched bytes
        pop     esi
        mov     bl,02H          ; set follows_literal to 2 (bx is unmodified by match commands)
        jmp     .next_command

        ; read gamma2-coded value into cx

.get_gamma2:
        xor     ecx,ecx         ; initialize to 1 so that value will start at 2
        inc     ecx             ; when shifted left in the adc below

.gamma2_loop:
        apl_get_bit             ; read data bit
        adc     ecx,ecx         ; shift into cx
        apl_get_bit             ; read continuation bit
        jc      .gamma2_loop    ; loop until a zero continuation bit is read

        ret

        ; handle 7 bits offset + 1 bit len or 4 bits offset / 1 byte copy

.other:
        xor     ecx,ecx
        apl_get_bit             ; read '7+1 match or short literal' bit
        jc      .short_literal  ; 111: 4 bit offset for 1-byte copy

                                ; 110: 7 bits offset + 1 bit length
                                
        movzx   edx,byte[esi]   ; read offset + length in dl
        inc     esi

        inc     ecx             ; prepare cx for length below
        shr     dl,1            ; shift len bit into carry, and offset in place
        je      .done           ; if zero offset: EOD
        adc     ecx,ecx         ; len in cx: 1*2 + carry bit = 2 or 3
        jmp     .got_len

        ; 4 bits offset / 1 byte copy

.short_literal:
        apl_get_bit             ; read 4 offset bits
        adc     ecx,ecx
        apl_get_bit
        adc     ecx,ecx
        apl_get_bit
        adc     ecx,ecx
        apl_get_bit
        adc     ecx,ecx
        xchg    eax,ecx         ; preserve bit queue in cx, put offset in ax
        jz      .write_zero     ; if offset is 0, write a zero byte

                                ; short offset 1-15
        mov     ebx,edi         ; point to destination in es:di - offset in ax
        sub     ebx,eax         ; we trash bx, it will be reset to 3 when we loop
        mov     al,[ebx]        ; read byte from short offset
.write_zero:
        stosb                   ; copy matched byte
        mov     eax,ecx         ; restore bit queue in al
        jmp     .next_command_after_literal

.done:
        sub     edi, [esp+32+8] ; compute decompressed size
        mov     [esp+28], edi
        popad
        ret
