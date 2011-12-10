ifndef VERSION
VERSION=v0.0.0.3
endif
ifndef RELEASE
RELEASE=0
endif
ifndef ARCH
ARCH=x86
endif
ifndef INSTALLDIR
INSTALLDIR=.
endif
ifndef USE_RENDERER_DLOPEN
USE_RENDERER_DLOPEN=1
endif
ifndef USE_OPENAL_DLOPEN
USE_OPENAL_DLOPEN=1
endif
ifndef USE_CURL_DLOPEN
USE_CURL_DLOPEN=0
endif
ifndef USE_INTERNAL_SPEEX
USE_INTERNAL_SPEEX=1
endif
ifndef USE_INTERNAL_ZLIB
USE_INTERNAL_ZLIB=1
endif
ifndef USE_INTERNAL_JPEG
USE_INTERNAL_JPEG=1
endif
 

ifeq ($(ARCH),x64)
  SDLDLL=SDL64.dll
else
  SDLDLL=SDL.dll
endif

DEFINES=
ifeq ($(USE_RENDERER_DLOPEN),1)
	DEFINES+= -DUSE_RENDERER_DLOPEN
endif
ifeq ($(USE_OPENAL_DLOPEN),1)
	DEFINES+= -DUSE_OPENAL_DLOPEN
endif
ifeq ($(USE_CURL_DLOPEN),1)
	DEFINES+= -DUSE_CURL_DLOPEN
endif
ifeq ($(USE_INTERNAL_SPEEX),1)
	DEFINES+= -DUSE_INTERNAL_SPEEX
endif
ifeq ($(USE_INTERNAL_ZLIB),1)
	DEFINES+= -DUSE_INTERNAL_ZLIB
endif
ifeq ($(USE_INTERNAL_JPEG),1)
	DEFINES+= -DUSE_INTERNAL_JPEG
endif


all: q3rally-$(VERSION)-$(RELEASE).$(ARCH).exe

q3rally.$(ARCH).nsi: q3rally.nsi.in
	sed 's/XXXVERSIONXXX/$(VERSION)/;s/XXXRELEASEXXX/$(RELEASE)/;s/x86/$(ARCH)/g;s/SDL.dll/$(SDLDLL)/' < $< > $@

q3rally-$(VERSION)-$(RELEASE).$(ARCH).exe: q3rally.$(ARCH).nsi
	makensis $(DEFINES) q3rally.$(ARCH).nsi

clean:
	rm -rf *.exe q3rally.$(ARCH).nsi

install:
	mkdir -p $(INSTALLDIR)
	mv q3rally-$(VERSION)-$(RELEASE).$(ARCH).exe $(INSTALLDIR)

.PHONY: all clean

