#ifndef sgi
.set gp=64
#endif
.set noreorder

.globl __cart_buf_rd
__cart_buf_rd:
	la      $5, __cart_buf
	addu    $6, $5, 512
1:
	uld     $8, 0($4)
	uld     $9, 8($4)
	sd      $8, 0($5)
	sd      $9, 8($5)
	addu    $5, 16
	bne     $5, $6, 1b
	addu    $4, 16
	j       $31
	nop

.globl __cart_buf_wr
__cart_buf_wr:
	la      $5, __cart_buf
	addu    $6, $5, 512
1:
	ld      $8, 0($5)
	ld      $9, 8($5)
	usd     $8, 0($4)
	usd     $9, 8($4)
	addu    $5, 16
	bne     $5, $6, 1b
	addu    $4, 16
	j       $31
	nop
