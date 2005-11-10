ifeq "$(TARGET_ARCH)$(UI_TYPE)" ""
	include $(ROOTDIR)/makefiles/target.mk
endif

ifeq "$(TARGET_ARCH)" ""
$(error TARGET_ARCH is not defined. Please edit $(ROOTDIR)/makefiles/target.mk)
endif

ifeq "$(UI_TYPE)" ""
$(error UI_TYPE is not defined. Please edit $(ROOTDIR)/makefiles/target.mk)
endif

ifeq "$(TARGET_STATUS)" ""
	TARGET_STATUS = release
endif
