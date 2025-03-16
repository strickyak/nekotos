all: \
  for-16k-cocoio \
  # notyet # for-16k-bonobo \
  ##

for-16k-cocoio:
	rm -rf build-$@
	mkdir build-$@
	sh create-submakefile.sh > build-$@/Makefile  \
        RAM_LIMIT=16384 NET_TYPE=cocoio
	make -C build-$@

for-16k-bonobo:
	rm -rf build-$@
	mkdir build-$@
	sh create-submakefile.sh > build-$@/Makefile  \
        RAM_LIMIT=16384 NET_TYPE=bonobo
	make -C build-$@

clean:
	rm -rf build-*/
