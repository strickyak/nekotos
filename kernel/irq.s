
;;; gcc for m6809 : Dec 25 2023 18:10:03
;;; 4.6.4 (gcc6809lw pl9)
;;; ABI version 1
;;; -mint16
	.module	irq.c

	.area	.text
_second_handler:
	ldd	-28658
	addd	#1
	std	-28658
	rts
_tick_handler:
	ldb	-28659
	cmpb	#8	;cmpqi:
	bhi	L3
	incb
	stb	-28659
	bra	L2
L3:
	clr	-28659
	jsr	[-28664]
	ldb	-28656
	cmpb	#58	;cmpqi:
	bhi	L5
	incb
	stb	-28656
	bra	L2
L5:
	clr	-28656
	ldb	-28655
	cmpb	#58	;cmpqi:
	bhi	L6
	incb
	stb	-28655
	bra	L2
L6:
	clr	-28655
	ldb	-28654
	cmpb	#22	;cmpqi:
	bhi	L7
	incb
	stb	-28654
	bra	L2
L7:
	clr	-28654
	ldb	-28644
	stb	-28653
	ldb	-28643
	stb	-28652
	ldb	-28642
	stb	-28651
	ldx	-28641
	stx	-28650
	ldb	-28639
	stb	-28648
	ldx	-28638
	stx	-28647
	ldb	-28636
	stb	-28645
L2:
	rts

	.area	.text.startup

	.globl	_main
_main:
	pshs	u
	ldx	#_tick_handler
	stx	-28666
	ldu	#_second_handler
	stu	-28664
	ldb	-28660
	cmpb	#4	;cmpqi:
	bhi	L9
	incb
	stb	-28660
	bra	L10
L9:
	clr	-28660
	jsr	,x
L10:
	ldx	#0
	puls	u,pc
