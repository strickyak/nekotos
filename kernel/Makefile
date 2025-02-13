all: _nekot.o

PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

_nekot.s: _FORCE_
	sh compile.sh

_nekot.o: _nekot.s
	time - lwasm $(PRAGMATA) --obj -o'_nekot.o' _nekot.s --list='_nekot.list' --map='_nekot.o.map' --symbol-dump='_nekot.o.sym'
	time - lwlink --decb --output='_nekot.decb' _nekot.o -L"$$HOME/coco-shelf/lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='_main' --script='script-0002-1002.txt' --map='_nekot.map'
	cp -vf  _nekot.decb  $$HOME/coco-shelf/build-frobio/pizga-base/Internal/LEMMINGS/test98.lem
	: : : ---- OKAY ---- : : :

_FORCE_:
