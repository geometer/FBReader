ALL_TARGET_ARCHS = zaurus pdaxrom-1.1.0rc10 pdaxrom-1.1.0rc11 desktop desktop-static palm maemo qvfb opensimpad-0.9.0

ifneq "$(wildcard $(ROOTDIR)/makefiles/target.mk)" ""
	include $(ROOTDIR)/makefiles/target.mk
endif

ifeq "$(TARGET_ARCH)" ""
$(error TARGET_ARCH is not defined. Please edit $(ROOTDIR)/makefiles/target.mk)
endif

ifeq "$(TARGET_STATUS)" ""
	TARGET_STATUS = release
endif

ifneq "$(findstring $(TARGET_ARCH),$(ALL_TARGET_ARCHS))" "$(TARGET_ARCH)"
$(error Unknown target $(TARGET_ARCH))
endif

# IMPORTANT: if you want to compile the stuff for your architecture
# please create in this directory a file called target.mk and put in it
# the line like
#
#	TARGET_ARCH = <your target arch>
#
# Available architectures are listed at the top of this file.
