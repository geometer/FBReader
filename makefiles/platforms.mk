ALL_TARGET_ARCHS = zaurus-qtopia zaurus-pdaxrom-qt zaurus-pdaxrom-gtk desktop-qt desktop-gtk palm maemo

ifneq "$(wildcard $(ROOTDIR)/makefiles/target.mk)" ""
	include $(ROOTDIR)/makefiles/target.mk
endif

ifeq "$(TARGET_ARCH)" ""
	TARGET_ARCH = zaurus-qtopia
$(warning TARGET_ARCH is not defined.  Using default -- $(TARGET_ARCH))
endif

ifneq "$(findstring $(TARGET_ARCH),$(ALL_TARGET_ARCHS))" "$(TARGET_ARCH)"
	$(error Unknown target $(TARGET_ARCH); valid targets are $(ALL_TARGET_ARCHS))
endif

# IMPORTANT: if you want to compile the stuff for your architecture
# please create in this directory a file called target.mk and put in it
# the line like
#
#	TARGET_ARCH = <your target arch>
#
# Available architectures are listed at the top of this file.
