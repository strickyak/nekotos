all: \
  BINARY \
  for-16k-cocoio \
  for-16k-bonobo \
  ##

BINARY:
	make -C mcp/mcp-bonobo-only all

for-16k-cocoio:
	rm -rf build-$@/*
	mkdir -p build-$@
	sh create-submakefile.sh > build-$@/Makefile $@ \
        RAM_LIMIT=16384 NET_TYPE=cocoio
	make -C build-$@
	make -C build-$@ install-test98

for-16k-bonobo:
	rm -rf build-$@/*
	mkdir -p build-$@
	sh create-submakefile.sh > build-$@/Makefile $@ \
        RAM_LIMIT=16384 NET_TYPE=bonobo
	make -C build-$@

clean:
	rm -rf build-*/
