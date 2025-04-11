KERNEL_PRODUCTS = _kernel.decb _kernel_sym.o _game.script

NEKOTOS_OUT_DIR = ../../build-frobio/pizga/Internal/NEKOTOS

all: ${KERNEL_PRODUCTS} ${GAMES}
	echo ALL OKAY

.SUFFIXES:

GCC = ../../bin/gcc6809 -std=gnu99 
GCFLAGS = -Os -I.. -f'omit-frame-pointer' -Wall $(DFLAGS)

LWASM = ../../bin/lwasm.orig --obj  \
      --pragma=undefextern --pragma=cescapes --pragma=importundefexport

LWLINK = ../../bin/lwlink --format=decb  \
      -L'../../lib/gcc/m6809-unknown/4.6.4/'

_kernel.c: $(CFILES)
	cat ../kernel/kernel.c > _kernel.c

_kernel.s: _kernel.c
	$(GCC) -f'whole-program' -S $(GCFLAGS) $<

_kernel.o: _kernel.s
	$(LWASM) $< -o'$@' --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_kernel.decb: _kernel.o
	$(LWLINK) $< -o'$@' --map='$@.map' --script=../kernel/kernel.script --entry=entry -lgcc
	grep '^Section:' $@.map
	sha256sum < $@ | dd bs=1 count=16 | ( cat ; echo '' ) > $@.hash
	cat $@.hash
	python3 -c "import sys; sys.stdout.buffer.write(bytes([0, 0, 8, 0x01, 0x18]))" > _hash.decb
	python3 -c "import sys; x=0x$$(cat $@.hash); sys.stdout.buffer.write(bytes([(255 & (x>>(8*(7-i)))) for i in range(8)]))" >> _hash.decb
	cat _hash.decb $@  > $@.new
	mv -fv $@.new $@
	cp -fv $@ _kernel.$$(cat $@.hash).decb
	test -d $(NEKOTOS_OUT_DIR) || mkdir $(NEKOTOS_OUT_DIR)
	cp -vf _kernel.decb $(NEKOTOS_OUT_DIR)/kernel.$(TARGET).decb
	cp -vf $@.hash $(NEKOTOS_OUT_DIR)/kernel.$(TARGET).hash
	echo OKAY $@

_kernel_sym.s : _kernel.decb
	awk ' \
            BEGIN {print "  .area .absolute"} \
            /^Symbol: _([gx][A-Z]|mem|str)/ { \
                printf "  .globl  %s\n%-20s EQU  $$%s\n", $$2, $$2, $$5} \
        ' < _kernel.decb.map > '$@'

_kernel_sym.o : _kernel_sym.s
	$(LWASM) -o'$@' $< --list='$@.list' --map='$@.map' --symbol-dump='$@.sym'

_game.script : _kernel.decb
	sh ../kernel/n1-create-game-script.sh < '$<.map' > '_game.script'

install-test98: _kernel.decb
	cp -vf  _kernel.decb  ../../build-frobio/pizga-base/Internal/LEMMINGS/test98.lem

################
