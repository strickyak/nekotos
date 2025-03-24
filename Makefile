all: \
  for-16k-cocoio \
  # notyet # for-16k-bonobo \
  ##

for-16k-cocoio:
	rm -rf build-$@/*
	mkdir -p build-$@
	sh create-submakefile.sh > build-$@/Makefile  \
        RAM_LIMIT=16384 NET_TYPE=cocoio
	make -C build-$@
	make -C build-$@ install

for-16k-bonobo:
	rm -rf build-$@/*
	mkdir -p build-$@
	sh create-submakefile.sh > build-$@/Makefile  \
        RAM_LIMIT=16384 NET_TYPE=bonobo
	make -C build-$@
	make -C build-$@ install

clean:
	rm -rf build-*/
