all: _target_.o

PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

_target_.s: _FORCE_
	sh compile.sh

_target_.o: _target_.s
	time - lwasm $(PRAGMATA) --obj -o'_target_.o' _target_.s --list='_target_.list' --map='_target_.map' --symbol-dump='_target_.sym'
	time - lwlink --decb --output='target.decb' _target_.o --entry='_main' --script='target.script' --map='target.map'
	cp -vf  target.decb  $$HOME/coco-shelf/build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

_FORCE_:
