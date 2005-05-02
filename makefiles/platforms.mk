ALL_TARGET_ARCHS = zaurus-cacko zaurus-pdaxrom-qt zaurus-pdaxrom-gtk desktop-qt desktop-gtk palm

ifneq "$(wildcard $(ROOTDIR)/makefiles/target.mk)" ""
	include $(ROOTDIR)/makefiles/target.mk
else
	TARGET_ARCH = zaurus-cacko
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
