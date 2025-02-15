all: _nekot.o

PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

_nekot.s: _FORCE_
	sh compile.sh

_nekot.o: _nekot.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_kernlimit.o' kernlimit.asm --list='_kernlimit.list' --map='_kernlimit.o.map' --symbol-dump='_kernlimit.o.sym'
	../../bin/lwasm $(PRAGMATA) --obj -o'_nekot.o' _nekot.s --list='_nekot.list' --map='_nekot.o.map' --symbol-dump='_nekot.o.sym'
	../../bin/lwlink --decb --output='_nekot.decb' _nekot.o _kernlimit.o -L"../../lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='_main' --script='script-0002-1002.txt' --map='_nekot.map'
	:
	awk '/^Symbol: _[A-Z]/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot.map > _nekot_sym.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_nekot_sym.o' _nekot_sym.s --list='_nekot_sym.list' --map='_nekot_sym.o.map' --symbol-dump='_nekot_sym.o.sym'
	sed '/^_Real/d' < _game.s > _tmp.s
	cat < _tmp.s > _game.s
	cat _nekot_sym.s >> _game.s
	../../bin/lwasm $(PRAGMATA) --obj -o'_game.o' _game.s --list='_game.list' --map='_game.o.map' --symbol-dump='_game.o.sym'
	:
	../../bin/lwlink --decb --output='_game.decb' _game.o -L"../../lib/gcc/m6809-unknown/4.6.4/" -lgcc --entry='_main' --script='script-2002.txt' --map='_game.map'
	:
	cp -vf  _nekot.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem
	: : : ---- OKAY ---- : : :
	: : : ---- OKAY ---- : : :
	: : : ---- OKAY ---- : : :

_FORCE_:
