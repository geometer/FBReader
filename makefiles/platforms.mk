ALL_TARGET_ARCHS = zaurus-qtopia zaurus-pdaxrom-qt zaurus-pdaxrom-gtk desktop-qt desktop-gtk palm maemo qvfb-opie opensimpad-opie

GTK_PLATFORMS = desktop-gtk zaurus-pdaxrom-gtk maemo
QT_PLATFORMS = desktop-qt zaurus-pdaxrom-qt zaurus-qtopia qvfb-opie opensimpad-opie

ifneq "$(wildcard $(ROOTDIR)/makefiles/target.mk)" ""
	include $(ROOTDIR)/makefiles/target.mk
endif

ifeq "$(TARGET_ARCH)" ""
$(error TARGET_ARCH is not defined. Please edit $(ROOTDIR)/makefiles/target.mk)
endif

ifeq "$(TARGET_STATUS)" ""
	TARGET_STATUS = release
endif

ifeq "$(findstring $(TARGET_ARCH),$(GTK_PLATFORMS))" "$(TARGET_ARCH)"
	UI_TYPE = gtk
endif

ifeq "$(findstring $(TARGET_ARCH),$(QT_PLATFORMS))" "$(TARGET_ARCH)"
	UI_TYPE = qt
endif

ifeq "$(TARGET_ARCH)" "palm"
	UI_TYPE = palm
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
