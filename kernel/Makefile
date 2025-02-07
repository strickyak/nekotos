all: _target_.o

_target_.s: _FORCE_
	sh compile.sh

_target_.o: _target_.s
	time - lwasm --obj -o'_target_.o' _target_.s --list='_target_.list' --map='_target_.map'

_FORCE_:
