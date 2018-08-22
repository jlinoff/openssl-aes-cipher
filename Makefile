# Build and test the cipher class.

define HDR
	@echo
	@/bin/echo -n -e "\033[1m"
	@echo "# ================================================ #"
	@echo $1 | awk '{printf("# %-48s #\n",$$0);}'
	@echo "# ================================================ #"
	@/bin/echo -n -e "\033[0m"
endef

# Build the tools and test the implementation.
.PHONY: all pkg test clean docs
all: test bin/ct.exe dbg/ct.exe docs

PKGVER=1.3.0
pkg: | doxydocs
	$(call HDR,$@)
	@umask 0 ;\
	rm -rf cipher-$(PKGVER)* ; \
	mkdir cipher-$(PKGVER) ; \
	cp $$(git ls-files) cipher-$(PKGVER) ;\
	tar jcf cipher-$(PKGVER).tar.bz2 cipher-$(PKGVER) ;\
	tar zcf cipher-$(PKGVER).tar.gz  cipher-$(PKGVER) ;\
	zip -r cipher-$(PKGVER).zip  cipher-$(PKGVER) ;\
	tar jcf cipher-$(PKGVER)-doxydocs.tar.bz2 doxydocs ; \
	sum cipher-$(PKGVER).tar.bz2 > cipher-$(PKGVER).tar.bz2.sum ; \
	sum cipher-$(PKGVER).tar.gz  > cipher-$(PKGVER).tar.gz.sum ; \
	sum cipher-$(PKGVER).zip     > cipher-$(PKGVER).zip.sum ; \
	rm -rf cipher-$(PKGVER)
	@ls -lh cipher-$(PKGVER)*

clean:
	$(call HDR,$@)
	rm -rf *~ *.exe *.o test_*.txt test_*.dat test src bin dbg \
		cipher-$(PKGVER)* doxydocs

scrub:
	git clean -f -d -x

test: bin/test.exe dbg/ct.exe | test.txt
	$(call HDR,$@)
	./bin/test.exe
	@[ -d test ] && rm -rf test || true
	@mkdir test
	@/bin/echo -e "\033[1mTest openssl decrypt compatibility\033[0m"
	dbg/ct.exe -e -x 0102030405060708 -D md5 -p password -i test.txt -o test/test1.out
	openssl aes-256-cbc -d -k password -salt -S 0102030405060708 -a -md md5 -in test/test1.out -out test/test1.out.txt
	diff test.txt test/test1.out.txt
	@/bin/echo -e "\033[1mTest openssl encrypt compatibility\033[0m"
	openssl aes-256-cbc -e -k password -salt -S 0102030405060708 -a -md md5 -in test.txt -out test/test2.out
	dbg/ct.exe -d -x 0102030405060708 -D md5 -p password -i test/test2.out -o test/test2.txt.out
	diff test.txt test/test2.txt.out
	@/bin/echo -e "\033[32;1mTESTS PASSED\033[0m"

docs: doxydocs

doxydocs:
	$(call HDR,$@)
	@if [ ! -d src ] ; then umask 0; mkdir src; fi
	cp cipher.cc cipher.h a.h src/
	doxygen doxygen.cfg

bin/%.o : %.cc cipher.h
	$(call HDR,$@)
	@if [ ! -d bin ] ; then mkdir bin; fi
	$(CXX) -Wall -Wno-deprecated-declarations -O2 -c -o $@ $<

bin/%.exe : bin/%.o bin/cipher.o
	$(call HDR,$@)
	$(CXX) -Wall -O2 -o $@ $< bin/cipher.o -lssl -lcrypto

dbg/%.o : %.cc cipher.h
	$(call HDR,$@)
	@if [ ! -d dbg ] ; then mkdir dbg; fi
	$(CXX) -Wall -Wno-deprecated-declarations -g -c -o $@ $<

dbg/%.exe : dbg/%.o dbg/cipher.o
	$(call HDR,$@)
	$(CXX) -Wall -g -o $@ $< dbg/cipher.o -lssl -lcrypto

