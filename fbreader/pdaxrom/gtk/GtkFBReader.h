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
	void refresh();
	void close();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

public:
	void handleKeyEventSlot(GdkEventKey*);

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
	ZLIntegerRangeOption myWidthOption;
	ZLIntegerRangeOption myHeightOption;

	GtkWindow *myMainWindow;
	GtkWidget *myToolbar;
	GtkWidget *myVBox;

	std::map<ZLApplication::Toolbar::ItemPtr,GtkWidget*> myButtons;

	bool myFullScreen;
};

#endif /* __GTKAPPLICATIONWINDOW_H__ */

// vim:ts=2:sw=2:noet
