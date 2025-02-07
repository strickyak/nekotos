all: _target_.o

_target_.s: _FORCE_
	sh compile.sh

_target_.o: _target_.s
	: sed -e 's/[.]area/;;; &/' -e 's/[.]globl/;;; &/' _target_.s > _z.s
	: mv _z.s _target_.s 
	time - lwasm --obj -o'_target_.o' _target_.s --list='_target_.list' --map='_target_.map' --symbol-dump='_target_.sym'
	time - lwlink --decb --output='target.decb' _target_.o --entry=_main

_FORCE_:
