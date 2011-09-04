include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /usr
IMAGEDIR = $(INSTALLDIR)/share/%APPLICATION_NAME%/icons
APPIMAGEDIR = $(IMAGEDIR)

CC = ccache gcc
AR = ar rsu

DEPGEN = $(CC) -MM
RM = rm -rvf
RM_QUIET = rm -rf

LD = g++
ifeq "$(UI_TYPE)" "qml" # Meego 1.2 Harmattan
	CC = gcc
	CFLAGS = -DMEEGO_VERSION_MAJOR=1 -DMEEGO_VERSION_MINOR=2 -DMEEGO_VERSION_PATCH=0 -DMEEGO_EDITION=harmattan -DMEEGO_EDITION_HARMATTAN
	CFLAGS += -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
	MOC = moc-qt4
	QTINCLUDE = -I/usr/include/qt4 -I/usr/include/qt4/QtDeclarative -I/usr/include/qt4/QtScript -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtOpenGL -I/usr/include/resource/qt4
	UILIBS = -lQtCore -lQtGui -lQtDeclarative -lQtOpenGL -lresourceqt
else ifeq "$(UI_TYPE)" "maemo5"
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -pthread -DMAEMO_VERSION=5
	GTKINCLUDE = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12 -DMAEMO_CHANGES -I/usr/include/hildon-1 -I/usr/include/hildon-fm-2 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/conic
	UILIBS = -lhildon-1 -lhildonfm -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgmodule-2.0 -ldl -lfreetype -lz -lfontconfig -lpng12 -lXrender -lX11 -losso -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 -lconic
else ifeq "$(UI_TYPE)" "maemo4"
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -pthread -mthumb -DMAEMO_VERSION=4
	GTKINCLUDE = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12 -DMAEMO_CHANGES -I/usr/include/hildon-1 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/conic
	UILIBS = -lhildon-1 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgmodule-2.0 -ldl -lfreetype -lz -lfontconfig -lpng12 -lXrender -lX11 -losso -ldbus-glib-1 -ldbus-1 -lgobject-2.0 -lglib-2.0 -lconic
else # ifeq "$(UI_TYPE)" "maemo2/3"
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -pthread -mthumb
	GTKINCLUDE = -I/usr/include/libxml2 -I/usr/include/libglade-2.0 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/X11R6/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/hildon-widgets -I/usr/include/hildon-lgpl -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/conic
	UILIBS = -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lm -lgmodule-2.0 -ldl -lgthread-2.0 -lhildonwidgets -losso
  ifeq "$(UI_TYPE)" "maemo2"
		CFLAGS += -DMAEMO_VERSION=2
  else # ifeq "$(UI_TYPE)" "maemo3"
		CFLAGS += -DMAEMO_VERSION=3
		UILIBS += -lconic
  endif
endif
