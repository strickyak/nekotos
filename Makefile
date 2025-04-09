all: \
  binaries \
  for-16k-cocoio \
  for-16k-bonobo \
  ##

binaries:
	rm -f ../bin/tether
	cd ../copico-bonobo && GOBIN=$$PWD/../bin go install -x v2.4/tether/tether.go
	rm -f ../bin/mcp-bonobo-only
	GOBIN=$$PWD/../bin go install -x mcp/mcp-bonobo-only/mcp-bonobo-only.go
	ls -l ../bin/tether
	ls -l ../bin/mcp-bonobo-only

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
