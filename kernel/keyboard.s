
;;; gcc for m6809 : Dec 25 2023 18:10:03
;;; 4.6.4 (gcc6809lw pl9)
;;; ABI version 1
;;; -mint16
	.module	keyboard.c

	.area	.text
_kbd_scanner_changed:
	pshs	y,u
	leas	-2,s
	leau	,x
	clr	-254
	ldb	-256
	comb
	andb	#127
	beq	L2
	ldb	#9
	stb	,s
	ldb	#1
	stb	1,s
L3:
	ldb	1,s
	comb
	stb	-254
	ldb	-256
	comb
	andb	#127
	stb	,x+
	asl	1,s
	dec	,s
	ldb	,s
	bne	L3
L2:
	ldd	#8
	std	,--s
	ldd	#0
	std	,--s
	leax	,u
	jsr	_memset
	leax	,u
	leas	4,s
	ldu	8,s
	ldb	#4
L5:
	ldy	,x++
	cmpy	,u++	;cmphi:
	bne	L6
	decb
	bne	L5
	bra	L4
L6:
	ldb	#1
L4:
	leas	2,s
	puls	y,u,pc

	.area	.text.startup

	.globl	_main
_main:
	leas	-2,s
	ldb	_kbd_on_irq_counter
	clra		;zero_extendqihi: R:b -> R:d
	std	,s
	anda	#0
	andb	#1
	cmpd	#0	;cmphi:
	beq	L10
	ldx	#_Keyboard+8
	stx	,--s
	ldx	#_Keyboard
	jsr	_kbd_scanner_changed
	leas	2,s
	ldx	#_Keyboard
	tstb	;cmpqi:(ZERO)
	beq	L11
	bra	L12
L10:
	ldx	#_Keyboard
	stx	,--s
	ldx	#_Keyboard+8
	jsr	_kbd_scanner_changed
	leas	2,s
	tstb	;cmpqi:(ZERO)
	beq	L11
	ldx	#_Keyboard+8
L12:
	jsr	_SendKeyboardPacket
L11:
	inc	_kbd_on_irq_counter
	ldx	#0
	leas	2,s
	rts

	.area	.bss
_kbd_on_irq_counter:	.blkb	1
_Keyboard:	.blkb	16
