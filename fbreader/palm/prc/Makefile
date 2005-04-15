NAME = FBReader
APP = ../../$(NAME)
APPID = PBK5
RCP = $(NAME).rcp
PRC = $(NAME).prc
GRC = $(NAME).grc
BIN = $(NAME).bin

PILRC = pilrc -I ../../icons/palm
OBJRES = m68k-palmos-obj-res
BUILDPRC = build-prc

$(PRC): grc.stamp bin.stamp
	$(BUILDPRC) $(PRC) "$(NAME)" $(APPID) *.grc *.bin $(LINKFILES) 
	ls -l *.prc

grc.stamp: $(APP)
	$(OBJRES) $(APP)
	touch $@

bin.stamp:	$(RCP)
	$(PILRC) $^ $(BINDIR)
	touch $@

clean:
	rm -rf *.bin *.grc *.prc *.stamp
