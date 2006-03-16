ROOTDIR = $(PWD)/../..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk
include $(MAKEFILESDIR)/subdir.mk

all: ModelTest

ModelTest: ModelTest.o
	g++ -o $@ $< `find ../common -name "*.o"` -L ../../zlibrary -lzlibrary -lbz2 -lz -lexpat -lenca

ModelTest.o: ModelTest.cpp
