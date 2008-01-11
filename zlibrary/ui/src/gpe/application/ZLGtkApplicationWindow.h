/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLGTKAPPLICATIONWINDOW_H__
#define __ZLGTKAPPLICATIONWINDOW_H__

#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtktoolbar.h>
#include <gtk/gtktoolitem.h>
#include <gtk/gtkwindow.h>

#include "../../../../core/src/application/ZLApplicationWindow.h"

class ZLGtkApplicationWindow : public ZLApplicationWindow { 

public:
	ZLGtkApplicationWindow(ZLApplication *application);
	~ZLGtkApplicationWindow();

protected:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void refresh();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }

	void setFullscreen(bool fullscreen);
	bool isFullscreen() const;

public:
	void handleKeyEventSlot(GdkEventKey *event);

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
	ZLIntegerRangeOption myWidthOption;
	ZLIntegerRangeOption myHeightOption;

	GtkWindow *myMainWindow;
	GtkToolbar *myToolbar;
	GtkWidget *myVBox;

	bool myFullScreen;

	std::map<ZLApplication::Toolbar::ItemPtr,GtkToolItem*> myButtons;
};

#endif /* __ZLGTKAPPLICATIONWINDOW_H__ */
