ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

BINDIR=$(INSTALLDIR)/bin

all: FBReader.desktop net.mawhrin.FBReader.services

FBReader.desktop: FBReader.desktop.in
	sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

net.mawhrin.FBReader.services: net.mawhrin.FBReader.services.in
	sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

clean:
