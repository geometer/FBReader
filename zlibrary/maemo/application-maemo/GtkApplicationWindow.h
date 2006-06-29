/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __GTKAPPLICATIONWINDOW_H__
#define __GTKAPPLICATIONWINDOW_H__

#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include <hildon-widgets/hildon-app.h>
#include <hildon-widgets/hildon-appview.h>
#include <libosso.h>

#include <abstract/ZLApplication.h>

class GtkApplicationWindow : public ZLApplicationWindow { 

public:
	static const std::string ImageDirectory;

public:
	GtkApplicationWindow(ZLApplication *application);
	~GtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void addMenubarItem(ZLApplication::Menubar::ItemPtr item);
	void addMenubarItem(GtkMenu *menu, ZLApplication::Menubar::ItemPtr item);
	void refresh();
	void close();

	void setCaption(const std::string &caption) { hildon_app_set_title(myApp, caption.c_str()); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

public:
	void handleKeyEventSlot(GdkEventKey*);
	HildonApp *getMainWindow() const { return myApp; }

private:
	HildonApp *myApp;
	HildonAppView *myAppView;
	GtkToolbar *myToolbar;
	GtkMenu *myMenu;

	bool myFullScreen;

	std::map<ZLApplication::Toolbar::ItemPtr,GtkToolItem*> myButtons;
	std::map<ZLApplication::Menubar::ItemPtr,GtkMenuItem*> myMenuItems;
};

#endif /* __GTKAPPLICATIONWINDOW_H__ */

// vim:ts=2:sw=2:noet
