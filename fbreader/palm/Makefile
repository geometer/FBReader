#Makefile for gcc PalmPilot crosscompiler m68k-palmos-gcc package.
# Change the strings from APP to BUILDPRC to match your own
# system configuration.
# Place the file into the developping directory.
# "make clean" removes all generated files, even the ".prc".
APP             =PalmFBReader
ICONTEXT        ="FBReader"
APPID           =PBK5
RCP             =$(APP).rcp
PRC             =$(APP).prc
SRC             =$(APP).cpp
GRC             =$(APP).grc
BIN             =$(APP).bin

CC              =m68k-palmos-g++
PILRC           =pilrc -I ../icons/palm
TXT2BITM        =txt2bitm
OBJRES          =m68k-palmos-obj-res
BUILDPRC        =build-prc

CFLAGS          =-O2 $(DEFINES) $(INCLUDES)

all:            $(PRC)

$(PRC):       grc.stamp bin.stamp;
	$(BUILDPRC) $(PRC) $(ICONTEXT) $(APPID) *.grc *.bin $(LINKFILES) 
	ls -l *.prc

grc.stamp:    $(APP) ;
	$(OBJRES) $(APP)
	touch $@

$(APP):       $(SRC:.c=.o) ;
	$(CC) $(CFLAGS) $^ -o $@

bin.stamp:    $(RCP) ;
	$(PILRC) $^ $(BINDIR)
	touch $@

%.o:  %.c ;
	$(CC) $(CFLAGS) -c $< -o $@
	#               touch $<
	# enable this line if you want to compile EVERY time.

depend dep: ;
	$(CC) -M $(SRC) > .dependencies


clean:
	rm -rf *.o $(APP) *.bin *.grc *.prc *.stamp
