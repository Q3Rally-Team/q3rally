VERSION=1.0_SVN
RELEASE=0
ARCH=x86

all: q3rally-$(VERSION)-$(RELEASE).$(ARCH).exe

q3rally.nsi: q3rally.nsi.in
	sed 's/XXXVERSIONXXX/$(VERSION)/;s/XXXRELEASEXXX/$(RELEASE)/;s/x86/$(ARCH)/g' < $< > $@

q3rally-$(VERSION)-$(RELEASE).$(ARCH).exe: q3rally.nsi
	makensis q3rally.nsi

clean:
	rm -rf *.exe q3rally.nsi

.PHONY: all clean
