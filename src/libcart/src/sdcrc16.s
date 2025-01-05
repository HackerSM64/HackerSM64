#ifndef sgi
.set gp=64
#endif
.set noreorder

.globl __sd_crc16
__sd_crc16:
	move    $25, $31
	li      $8, 0x00AA00AA
	li      $9, 0x0000CCCC
	li      $10, 0x55555555
	li      $11, 0x33333333
	li      $12, 0x0F0F0F0F
	li      $13, 0x00FF00FF
	li      $14, 0x0000FFFF
	dsll    $15, $8, 32
	or      $8, $15
	dsll    $15, $9, 32
	or      $9, $15
	dsll    $15, $10, 32
	or      $10, $15
	dsll    $15, $11, 32
	or      $11, $15
	dsll    $15, $12, 32
	or      $12, $15
	dsll    $15, $13, 32
	or      $13, $15
	dsll    $15, $14, 32
	or      $14, $15
	sd      $0, ($29)
	addu    $6, $5, 512-8
1:
	ld      $2, ($5)
	dsrl    $3, $2, 7
	xor     $3, $2
	and     $3, $8
	xor     $2, $3
	dsll    $3, 7
	xor     $2, $3
	dsrl    $3, $2, 14
	xor     $3, $2
	and     $3, $9
	xor     $2, $3
	dsll    $3, 14
	xor     $2, $3
	li      $15, 0x0F0F0F0F
	and     $3, $2, $15
	dsll    $15, 4
	dsrl    $7, $2, 28
	and     $7, $15
	or      $3, $7
	dsrl    $7, $2, 32
	and     $7, $15
	and     $2, $15
	dsrl    $2, 4
	bal     3f
	or      $2, $7
	addu    $7, $29, 8-2
2:
	lhu     $3, ($7)
	xor     $24, $2, $3
	srl     $24, 8
	and     $24, 0xFF
	srl     $15, $24, 4
	xor     $24, $15
	sll     $3, 8
	xor     $3, $24
	sll     $24, 5
	xor     $3, $24
	sll     $24, 7
	xor     $3, $24
	srl     $15, $3, 8
	xor     $24, $2, $15
	and     $24, 0xFF
	srl     $15, $24, 4
	xor     $24, $15
	sll     $3, 8
	xor     $3, $24
	sll     $24, 5
	xor     $3, $24
	sll     $24, 7
	xor     $3, $24
	sh      $3, ($7)
	dsrl    $2, 16
	bne     $7, $29, 2b
	subu    $7, 2
	bne     $5, $6, 1b
	addu    $5, 8
	lwu     $2, 0($29)
	bal     3f
	lwu     $3, 4($29)
	dsll    $3, $2, 32
	dsrl    $3, 32
	bal     3f
	dsrl    $2, 32
	j       $25
	sd      $2, ($4)
3:
	move    $24, $31
	bal     4f
	nop
	dsll    $7, $2, 1
	bal     4f
	move    $2, $3
	j       $24
	or      $2, $7
4:
	dsll    $15, $2, 16
	or      $2, $15
	and     $2, $14
	dsll    $15, $2, 8
	or      $2, $15
	and     $2, $13
	dsll    $15, $2, 4
	or      $2, $15
	and     $2, $12
	dsll    $15, $2, 2
	or      $2, $15
	and     $2, $11
	dsll    $15, $2, 1
	or      $2, $15
	j       $31
	and     $2, $10
