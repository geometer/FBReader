NAME = FBReader
APP = ../../$(NAME)
APPID = FBRd
RCP = $(NAME).rcp
PRC = $(NAME).prc

PILRC = pilrc -I ../../icons/palm
OBJRES = m68k-palmos-obj-res
BUILDPRC = build-prc

$(PRC): bin.stamp ../../FBReader
	@$(BUILDPRC) -o $(PRC) -n $(NAME) -c $(APPID) ../FBReader.def ../../FBReader *.bin $(LINKFILES) 
	@ls -l *.prc
	@cp $(PRC) ../../..

bin.stamp:	$(RCP) Makefile
	@$(PILRC) $<
	@touch $@

clean:
	@$(RM) *.bin *.prc *.stamp
