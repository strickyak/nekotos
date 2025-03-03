SFILES = prelude.s wall.s real.s irq.s breakkey.s kern.s vdg.s network.s wiznet.s console.s main.s
OFILES = prelude.o wall.o real.o irq.o breakkey.o kern.o vdg.o network.o wiznet.o console.o main.o
all: $(SFILES) $(OFILES) _nekot2.decb

.SUFFIXES:


CC = ../../bin/gcc6809
CFLAGS = -Os -std=gnu99 -I.. -S 

LWASM = ../../bin/lwasm.orig
LWLINK = ../../bin/lwlink
PRAGMATA = --pragma=undefextern --pragma=cescapes --pragma=importundefexport

%.s:%.c
	$(CC) -S $(CFLAGS) $<

%.o:%.s
	$(LWASM) $< $(PRAGMATA) --obj -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_nekot2.decb: $(OFILES)
	$(LWLINK) --format=decb $(OFILES) -o'$@' --map='$@.map' --script=kernel.script --entry=_main

clean:
	rm -r *.s *.o *.list *.sym *.map *.decb

ci:
	bash /usr/local/bin/ci-l *.c *.h *.script Makefile
