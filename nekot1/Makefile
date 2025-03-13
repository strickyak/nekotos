CFILES = prelude.c alloc64.c score.c wall.c real.c irq.c breakkey.c kern.c vdg.c network.c wiznet.c console.c keyboard.c main.c
SFILES = prelude.s alloc64.s score.s wall.s real.s irq.s breakkey.s kern.s vdg.s network.s wiznet.s console.s keyboard.s main.s
OFILES = prelude.o alloc64.o score.o wall.o real.o irq.o breakkey.o kern.o vdg.o network.o wiznet.o console.o keyboard.o main.o

all: $(SFILES) $(OFILES) _nekot2.decb _nekot1.decb red green blue life lib8 forth

.SUFFIXES:


GCC = ../../bin/gcc6809
GCFLAGS = -Os -std=gnu99 -I.. -f'omit-frame-pointer' -Wall

LWASM = ../../bin/lwasm.orig
LWLINK = ../../bin/lwlink -L'../../lib/gcc/m6809-unknown/4.6.4/'
ASM_PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

%.s:%.c
	$(GCC) -S $(GCFLAGS) $<

%.o:%.s
	$(LWASM) $< $(ASM_PRAGMATA) --obj -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_nekot2.decb: $(OFILES)
	$(LWLINK) --format=decb $(OFILES) -o'$@' --map='$@.map' --script=kernel.script --entry=_main -lgcc

_nekot1.c: $(CFILES)
	for x in $(CFILES); do echo "#line 1 \"$$x\""; cat $$x; done > _nekot1.c
_nekot1.s: _nekot1.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) $<
_nekot1.o: _nekot1.s
	$(LWASM) $< $(ASM_PRAGMATA) --obj -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'
_nekot1.decb: _nekot1.o
	$(LWLINK) --format=decb $< -o'$@' --map='$@.map' --script=kernel.script --entry=entry -lgcc
	awk 'BEGIN {print "  .area .absolute"} /^Symbol: _([gx][A-Z]|mem|str)/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot1.decb.map > _nekot1_sym.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_nekot1_sym.o' _nekot1_sym.s --list='_nekot1_sym.list' --map='_nekot1_sym.o.map' --symbol-dump='_nekot1_sym.o.sym'
	sh n1-create-game-script.sh < $@.map > _game.script
	grep '^Section:' $@.map
	cp -vf  _nekot1.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

red: _nekot1.decb
	python3 n1preprocess.py ../games/red/red.c _pre_red.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_red.c
	cat _nekot1_sym.s >> _pre_red.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_red.o' _pre_red.s --list='_red.list' --map='_red.o.map' --symbol-dump='_red.o.sym'
	$(LWLINK) --decb --output='_red.decb' _red.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_red.map'
	grep '^Section:' _red.map
	cp -vf _red.decb /tmp/_red.decb

green: _nekot1.decb
	python3 n1preprocess.py ../games/green/green.c _pre_green.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_green.c
	cat _nekot1_sym.s >> _pre_green.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_green.o' _pre_green.s --list='_green.list' --map='_green.o.map' --symbol-dump='_green.o.sym'
	$(LWLINK) --decb --output='_green.decb' _green.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_green.map'
	grep '^Section:' _green.map
	cp -vf _green.decb /tmp/_green.decb

blue: _nekot1.decb
	python3 n1preprocess.py ../games/blue/blue.c _pre_blue.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_blue.c
	cat _nekot1_sym.s >> _pre_blue.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_blue.o' _pre_blue.s --list='_blue.list' --map='_blue.o.map' --symbol-dump='_blue.o.sym'
	$(LWLINK) --decb --output='_blue.decb' _blue.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_blue.map'
	grep '^Section:' _blue.map
	cp -vf _blue.decb /tmp/_blue.decb

life: _nekot1.decb
	python3 n1preprocess.py ../games/life/life.c _pre_life.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_life.c
	cat _nekot1_sym.s >> _pre_life.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_life.o' _pre_life.s --list='_life.list' --map='_life.o.map' --symbol-dump='_life.o.sym'
	$(LWLINK) --decb --output='_life.decb' _life.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_life.map'
	grep '^Section:' _life.map
	cp -vf _life.decb /tmp/_life.decb

lib8: _nekot1.decb
	python3 n1preprocess.py ../games/lib8/lib8.c _pre_lib8.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_lib8.c
	cat _nekot1_sym.s >> _pre_lib8.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_lib8.o' _pre_lib8.s --list='_lib8.list' --map='_lib8.o.map' --symbol-dump='_lib8.o.sym'
	$(LWLINK) --decb --output='_lib8.decb' _lib8.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_lib8.map'
	grep '^Section:' _lib8.map
	cp -vf _lib8.decb /tmp/_lib8.decb

forth: _nekot1.decb
	python3 n1preprocess.py ../games/forth/forth.c _pre_forth.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) -I../games/forth _pre_forth.c
	cat _nekot1_sym.s >> _pre_forth.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_forth.o' _pre_forth.s --list='_forth.list' --map='_forth.o.map' --symbol-dump='_forth.o.sym'
	$(LWLINK) --decb --output='_forth.decb' _forth.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_forth.map'
	grep '^Section:' _forth.map
	cp -vf _forth.decb /tmp/_forth.decb

spacewar: _nekot1.decb
	python3 n1preprocess.py ../games/spacewar/spacewar.c _pre_spacewar.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) -I../games/spacewar _pre_spacewar.c
	cat _nekot1_sym.s >> _pre_spacewar.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_spacewar.o' _pre_spacewar.s --list='_spacewar.list' --map='_spacewar.o.map' --symbol-dump='_spacewar.o.sym'
	$(LWLINK) --decb --output='_spacewar.decb' _spacewar.o -lgcc --entry='__n1pre_entry' --script='_game.script' --map='_spacewar.map'
	grep '^Section:' _spacewar.map
	cp -vf _spacewar.decb /tmp/_spacewar.decb

clean:
	rm -f *.s *.o *.list *.sym *.map *.decb _* ,*

ci:
	bash /usr/local/bin/ci-l *.c *.h *.script *.py *.awk Makefile
