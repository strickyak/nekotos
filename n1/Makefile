CFILES = prelude.c wall.c real.c irq.c breakkey.c kern.c vdg.c network.c wiznet.c console.c keyboard.c main.c
SFILES = prelude.s wall.s real.s irq.s breakkey.s kern.s vdg.s network.s wiznet.s console.s keyboard.s main.s
OFILES = prelude.o wall.o real.o irq.o breakkey.o kern.o vdg.o network.o wiznet.o console.o keyboard.o main.o
all: $(SFILES) $(OFILES) _nekot2.decb _nekot1.decb

.SUFFIXES:


GCC = ../../bin/gcc6809
GCFLAGS = -Os -std=gnu99 -I.. -S -f'omit-frame-pointer'

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
	grep [.]final[.] $@.map
	cp -vf  _nekot1.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

clean:
	rm -f *.s *.o *.list *.sym *.map *.decb _* ,*

ci:
	bash /usr/local/bin/ci-l *.c *.h *.script Makefile
