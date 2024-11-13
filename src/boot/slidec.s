/*
 * MIPS YAY0 decoder created by devwizard64.
 */

.set noat      # allow manual use of $at
.set gp=64
.set noreorder

.include "macros.inc"

.align 4

.text
glabel slidstart

slidstart:
	lw      $6, 4($4)
	add     $6, $5, $6
	move    $8, $0
1:
	bnez    $8, 2f
	add     $4, 1
	lwl     $9, 15($4)
	add     $4, 1
	li      $8, 8
2:
	bgez    $9, 2f
	lbu     $10, 15($4)
	sb      $10, ($5)
	b       3f
	add     $5, 1
2:
	add     $4, 1
	lbu     $11, 15($4)
	sll     $10, 8
	or      $10, $11
	srl     $11, $10, 12
	and     $10, 0xFFF
	bnez    $11, 2f
	add     $11, 2
	add     $4, 1
	lbu     $11, 15($4)
	add     $11, 18
2:
	sub     $10, $5, $10
	add     $11, $5, $11
2:
	lbu     $12, -1($10)
	add     $10, 1
	add     $5, 1
	bne     $5, $11, 2b
	sb      $12, -1($5)
3:
	sll     $9, 1
	bne     $5, $6, 1b
	sub     $8, 1
	j       $31
	nop
