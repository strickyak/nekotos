CFILES = prelude.c alloc64.c score.c wall.c real.c irq.c breakkey.c kern.c vdg.c network.c wiznet.c console.c keyboard.c main.c
SFILES = prelude.s alloc64.s score.s wall.s real.s irq.s breakkey.s kern.s vdg.s network.s wiznet.s console.s keyboard.s main.s
OFILES = prelude.o alloc64.o score.o wall.o real.o irq.o breakkey.o kern.o vdg.o network.o wiznet.o console.o keyboard.o main.o
all: $(SFILES) $(OFILES) _nekot2.decb _kernel.decb red green blue life

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

_kernel.c: $(CFILES)
	for x in $(CFILES); do echo "#line 1 \"$$x\""; cat $$x; done > _kernel.c
_kernel.s: _kernel.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) $<
_kernel.o: _kernel.s
	$(LWASM) $< $(ASM_PRAGMATA) --obj -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'
_kernel.decb: _kernel.o
	$(LWLINK) --format=decb $< -o'$@' --map='$@.map' --script=kernel.script --entry=entry -lgcc
	awk 'BEGIN {print "  .area .absolute"} /^Symbol: _[gx][A-Z]/ {printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5}' < _kernel.decb.map > _kernel_sym.s
	grep '^Section:' $@.map
	cp -vf  _kernel.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

red:
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_kernel_sym.o' _kernel_sym.s --list='_kernel_sym.list' --map='_kernel_sym.o.map' --symbol-dump='_kernel_sym.o.sym'
	python3 n1preprocess.py ../games/red/red.c _pre_red.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_red.c
	cat _kernel_sym.s >> _pre_red.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_red.o' _pre_red.s --list='_red.list' --map='_red.o.map' --symbol-dump='_red.o.sym'
	$(LWLINK) --decb --output='_red.decb' _red.o -lgcc --entry='__n1pre_entry' --script='red.script' --map='_red.map'
	grep '^Section:' _red.map
	cp -vf _red.decb /tmp/_red.decb

green:
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_kernel_sym.o' _kernel_sym.s --list='_kernel_sym.list' --map='_kernel_sym.o.map' --symbol-dump='_kernel_sym.o.sym'
	python3 n1preprocess.py ../games/green/green.c _pre_green.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_green.c
	cat _kernel_sym.s >> _pre_green.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_green.o' _pre_green.s --list='_green.list' --map='_green.o.map' --symbol-dump='_green.o.sym'
	$(LWLINK) --decb --output='_green.decb' _green.o -lgcc --entry='__n1pre_entry' --script='green.script' --map='_green.map'
	grep '^Section:' _green.map
	cp -vf _green.decb /tmp/_green.decb

blue:
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_kernel_sym.o' _kernel_sym.s --list='_kernel_sym.list' --map='_kernel_sym.o.map' --symbol-dump='_kernel_sym.o.sym'
	python3 n1preprocess.py ../games/blue/blue.c _pre_blue.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_blue.c
	cat _kernel_sym.s >> _pre_blue.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_blue.o' _pre_blue.s --list='_blue.list' --map='_blue.o.map' --symbol-dump='_blue.o.sym'
	$(LWLINK) --decb --output='_blue.decb' _blue.o -lgcc --entry='__n1pre_entry' --script='blue.script' --map='_blue.map'
	grep '^Section:' _blue.map
	cp -vf _blue.decb /tmp/_blue.decb

life:
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_kernel_sym.o' _kernel_sym.s --list='_kernel_sym.list' --map='_kernel_sym.o.map' --symbol-dump='_kernel_sym.o.sym'
	python3 n1preprocess.py ../games/life/life.c _pre_life.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) _pre_life.c
	cat _kernel_sym.s >> _pre_life.s
	$(LWASM) $(ASM_PRAGMATA) --obj -o'_life.o' _pre_life.s --list='_life.list' --map='_life.o.map' --symbol-dump='_life.o.sym'
	$(LWLINK) --decb --output='_life.decb' _life.o -lgcc --entry='__n1pre_entry' --script='life.script' --map='_life.map'
	grep '^Section:' _life.map
	cp -vf _life.decb /tmp/_life.decb

clean:
	rm -f *.s *.o *.list *.sym *.map *.decb _* ,*

ci:
	bash /usr/local/bin/ci-l *.c *.h *.script *.py *.awk Makefile
