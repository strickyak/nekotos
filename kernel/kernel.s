
;;; gcc for m6809 : Dec 25 2023 18:10:03
;;; 4.6.4 (gcc6809lw pl9)
;;; ABI version 1
;;; -mint16
	.module	kernel.c

	.area	.text.startup

	.globl	_main
_main:
	leas	-5,s
	clr	-254
	ldb	-256
	comb
	andb	#127
	lbeq	L2
	ldx	#_kbd_matrix
L3:
	ldb	#-2
	stb	-254
	ldb	-256
	comb
	andb	#127
	stb	,x+
	cmpx	#_kbd_matrix+9	;cmphi:
	bne	L3
	ldb	_kbd_matrix
	stb	1,s
	bne	L8
	ldb	_kbd_matrix+1
	stb	1,s
	bne	L9
	ldb	_kbd_matrix+2
	stb	1,s
	beq	L2
	ldb	#2
	bra	L4
L8:
	clrb
	bra	L4
L9:
	ldb	#1
L4:
	aslb
	aslb
	aslb
	stb	3,s
	ldb	#1
	stb	2,s
	stb	,s
L6:
	ldb	2,s
	andb	1,s
	bne	L12
	asl	2,s
	inc	,s
	ldb	,s
	cmpb	#9	;cmpqi:
	bne	L6
	clrb
	bra	L5
L12:
	ldb	,s
L5:
	addb	3,s
	bra	L7
L2:
	clr	_kbd_value
	clr	_kbd_modified
	clrb
L7:
	stb	28672
	ldx	#0
	leas	5,s
	rts

	.area	.bss
_kbd_matrix:	.blkb	8
_kbd_value:	.blkb	1
_kbd_modified:	.blkb	1
