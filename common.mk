KERNEL_PRODUCTS = _nekot1.decb _nekot1_sym.o _game.script

all: ${KERNEL_PRODUCTS} ${GAMES}
	echo ALL OKAY

.SUFFIXES:

GCC = ../../bin/gcc6809 -std=gnu99 
GCFLAGS = -Os -I.. -f'omit-frame-pointer' -Wall $(DFLAGS)

LWASM = ../../bin/lwasm.orig --obj  \
      --pragma=undefextern --pragma=cescapes --pragma=importundefexport

LWLINK = ../../bin/lwlink --format=decb  \
      -L'../../lib/gcc/m6809-unknown/4.6.4/'

_nekot1.c: $(CFILES)
	for x in $(CFILES); do \
            echo "#line 1 \"nekot1/$$(basename $$x)\""; \
            cat $$x; \
        done > _nekot1.c

_nekot1.s: _nekot1.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) $<

_nekot1.o: _nekot1.s
	$(LWASM) $< -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_nekot1.decb: _nekot1.o
	$(LWLINK) $< -o'$@' --map='$@.map' --script=../nekot1/kernel.script --entry=entry -lgcc
	grep '^Section:' $@.map
	sha256sum < $@ | dd bs=1 count=10 | ( cat ; echo '' ) > $@.hash
	cat $@.hash
	cp -av $@ nekot1.$$(cat $@.hash).decb
	echo OKAY $@

_nekot1_sym.s : _nekot1.decb
	awk ' \
            BEGIN {print "  .area .absolute"} \
            /^Symbol: _([gx][A-Z]|mem|str)/ { \
                printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5} \
        ' < _nekot1.decb.map > '$@'

_nekot1_sym.o : _nekot1_sym.s
	$(LWASM) -o'$@' $< --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_game.script : _nekot1.decb
	sh ../nekot1/n1-create-game-script.sh < '$<.map' > '_game.script'

install-test98: _nekot1.decb
	cp -vf  _nekot1.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

################
