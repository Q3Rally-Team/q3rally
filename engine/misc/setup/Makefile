VERSION=1.0_SVN
RELEASE=0

all:
	VERSION=$(VERSION) RELEASE=$(RELEASE) ./doit

sign:
	for i in *.run; do \
		gpg -bao $$i.asc $$i; \
	done

clean:
	rm -rf *.run image defines.m4

.PHONY: all sign clean
