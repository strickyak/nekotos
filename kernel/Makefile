all: _nekot.o

PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

_nekot.s: _FORCE_
	sh compile.sh

_nekot.o: _nekot.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_nekot.o' _nekot.s --list='_nekot.list' --map='_nekot.o.map' --symbol-dump='_nekot.o.sym'
	../../bin/lwlink --decb --output='_nekot.decb' _nekot.o -L"../../lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='entry' --script='kernel.script' --map='_nekot.map'
	:
	:
	awk '/^Symbol: _[A-Z]/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot.map > _nekot_sym.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_nekot_sym.o' _nekot_sym.s --list='_nekot_sym.list' --map='_nekot_sym.o.map' --symbol-dump='_nekot_sym.o.sym'
	sed '/^_Real/d' < _blue.s > _tmp.s
	cat < _tmp.s > _blue.s
	cat _nekot_sym.s >> _blue.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_blue.o' _blue.s --list='_blue.list' --map='_blue.o.map' --symbol-dump='_blue.o.sym'
	../../bin/lwlink --decb --output='_blue.decb' _blue.o -L"../../lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='_main' --script='blue.script' --map='_blue.map'
	cp -vf  _nekot.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem
	cp -vf  _blue.decb  /tmp/
	:
	:
	awk '/^Symbol: _[A-Z]/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot.map > _nekot_sym.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_nekot_sym.o' _nekot_sym.s --list='_nekot_sym.list' --map='_nekot_sym.o.map' --symbol-dump='_nekot_sym.o.sym'
	sed '/^_Real/d' < _green.s > _tmp.s
	cat < _tmp.s > _green.s
	cat _nekot_sym.s >> _green.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_green.o' _green.s --list='_green.list' --map='_green.o.map' --symbol-dump='_green.o.sym'
	../../bin/lwlink --decb --output='_green.decb' _green.o -L"../../lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='_main' --script='green.script' --map='_green.map'
	cp -vf  _nekot.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem
	cp -vf  _green.decb  /tmp/
	:
	:
	: : : ---- OKAY ---- : : :
	: : : ---- OKAY ---- : : :
	: : : ---- OKAY ---- : : :

_FORCE_:
