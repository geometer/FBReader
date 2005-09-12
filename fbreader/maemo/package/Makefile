ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

BINDIR=$(INSTALLDIR)/bin

all: FBReader.desktop

FBReader.desktop: FBReader.desktop.in
	sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

clean:
