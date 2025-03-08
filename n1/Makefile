CFILES = prelude.c wall.c real.c irq.c breakkey.c kern.c vdg.c network.c wiznet.c console.c keyboard.c main.c
SFILES = prelude.s wall.s real.s irq.s breakkey.s kern.s vdg.s network.s wiznet.s console.s keyboard.s main.s
OFILES = prelude.o wall.o real.o irq.o breakkey.o kern.o vdg.o network.o wiznet.o console.o keyboard.o main.o
all: $(SFILES) $(OFILES) _nekot2.decb _nekot1.decb

.SUFFIXES:


GCC = ../../bin/gcc6809
GCFLAGS = -Os -std=gnu99 -I.. -f'omit-frame-pointer'
GCFLAGS_GRAB = -std=gnu99 -I..

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
	grep '^Section:' $@.map
	cp -vf  _nekot1.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

red:
	awk 'BEGIN {print "  .area .absolute"} /^Symbol: _N1/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot1.decb.map > _nekot1_sym.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_nekot1_sym.o' _nekot1_sym.s --list='_nekot1_sym.list' --map='_nekot1_sym.o.map' --symbol-dump='_nekot1_sym.o.sym'
	python3 n1preprocess.py ../games/red/red.c _pre_red.c
	$(GCC) -S $(GCFLAGS_GRAB) _pre_red.c
	awk -f n1grab.awk < _pre_red.s > _grab_red.txt
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_red.c
	cat _nekot1_sym.s >> _pre_red.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_red.o' _pre_red.s --list='_red.list' --map='_red.o.map' --symbol-dump='_red.o.sym'
	$(LWLINK) --decb --output='_red.decb' _red.o -lgcc --entry='__n1pre_entry' --script='red.script' --map='_red.map'
	grep '^Section:' _red.map
	cp -vf _red.decb /tmp/_red.decb

green:
	awk 'BEGIN {print "  .area .absolute"} /^Symbol: _N1/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot1.decb.map > _nekot1_sym.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_nekot1_sym.o' _nekot1_sym.s --list='_nekot1_sym.list' --map='_nekot1_sym.o.map' --symbol-dump='_nekot1_sym.o.sym'
	python3 n1preprocess.py ../games/green/green.c _pre_green.c
	$(GCC) -S $(GCFLAGS_GRAB) _pre_green.c
	awk -f n1grab.awk < _pre_green.s > _grab_green.txt
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_green.c
	cat _nekot1_sym.s >> _pre_green.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_green.o' _pre_green.s --list='_green.list' --map='_green.o.map' --symbol-dump='_green.o.sym'
	$(LWLINK) --decb --output='_green.decb' _green.o -lgcc --entry='__n1pre_entry' --script='green.script' --map='_green.map'
	grep '^Section:' _green.map
	cp -vf _green.decb /tmp/_green.decb

blue:
	awk 'BEGIN {print "  .area .absolute"} /^Symbol: _N1/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _nekot1.decb.map > _nekot1_sym.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_nekot1_sym.o' _nekot1_sym.s --list='_nekot1_sym.list' --map='_nekot1_sym.o.map' --symbol-dump='_nekot1_sym.o.sym'
	python3 n1preprocess.py ../games/blue/blue.c _pre_blue.c
	$(GCC) -S $(GCFLAGS_GRAB) _pre_blue.c
	awk -f n1grab.awk < _pre_blue.s > _grab_blue.txt
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_blue.c
	cat _nekot1_sym.s >> _pre_blue.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_blue.o' _pre_blue.s --list='_blue.list' --map='_blue.o.map' --symbol-dump='_blue.o.sym'
	$(LWLINK) --decb --output='_blue.decb' _blue.o -lgcc --entry='__n1pre_entry' --script='blue.script' --map='_blue.map'
	grep '^Section:' _blue.map
	cp -vf _blue.decb /tmp/_blue.decb



clean:
	rm -f *.s *.o *.list *.sym *.map *.decb _* ,*

ci:
	bash /usr/local/bin/ci-l *.c *.h *.script *.py *.awk Makefile
