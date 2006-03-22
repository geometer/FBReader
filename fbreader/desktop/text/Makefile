ROOTDIR = $(PWD)/../../..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk
include $(MAKEFILESDIR)/subdir.mk

all: rmold run

rmold:
	@rm ModelTest

run: ModelTest
	@./ModelTest test1.fb2 > test1.tmp
	@cmp test1.tmp test1.out
	@./ModelTest test2.html > test2.tmp
	@cmp test2.tmp test2.out
	@./ModelTest test3.fb2 > test3.tmp
	@cmp test3.tmp test3.out

ModelTest: ModelTest.o
	@g++ -o $@ $< `find ../../common -name "*.o"` -L ../../../zlibrary -lzlibrary -lbz2 -lz -lexpat -lenca

ModelTest.o: ModelTest.cpp
