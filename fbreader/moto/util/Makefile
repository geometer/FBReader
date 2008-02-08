ROOTDIR = ../../..

TARGET = language_detector

include $(ROOTDIR)/makefiles/qsubdir.mk

$(TARGET): language_detector.o
	@$(LD) $(LDFLAGS) -o $@ $^ $(UILIBS)

distclean: clean
	@$(RM) $(TARGET)
