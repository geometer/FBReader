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

#include <vector>
#include <map>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtktoolbar.h>

#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"
#include "../optionView/ZLGtkOptionViewHolder.h"

class ZLOptionView;

class ZLGtkApplicationWindow : public ZLDesktopApplicationWindow { 

public:
	ZLGtkApplicationWindow(ZLApplication *application);
	~ZLGtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void init();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
	void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);

public:
	bool handleKeyEventSlot(GdkEventKey *event);
	void handleScrollEventSlot(GdkEventScroll *event);
	void onGtkButtonPress(GtkWidget *gtkButton);

	GtkWindow *getMainWindow() { return myMainWindow; }

private:
	GtkWindow *myMainWindow;

	GtkWidget *myVBox;

	GdkCursor *myHyperlinkCursor;
	bool myHyperlinkCursorIsUsed;

	class Toolbar : public ZLGtkOptionViewHolder {

	public:
		Toolbar(ZLGtkApplicationWindow *window);

		GtkWidget *toolbarWidget() const;

		void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
		void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
		void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);
		ZLApplication::Toolbar::ButtonItem &buttonItemByWidget(GtkWidget *gtkButton);

	private:
		void attachWidget(ZLOptionView &view, GtkWidget *widget);
		void attachWidgets(ZLOptionView &view, GtkWidget *widget0, GtkWidget *widget1);

	private:
		ZLGtkApplicationWindow *myWindow;
		GtkToolbar *myGtkToolbar;
		std::map<const ZLApplication::Toolbar::Item*,GtkWidget*> myButtonToWidget;
		std::map<GtkWidget*,ZLApplication::Toolbar::ItemPtr> myWidgetToButton;
		std::map<ZLApplication::Toolbar::ItemPtr,int> mySeparatorMap;
		std::vector<std::pair<ZLApplication::Toolbar::ItemPtr,bool> > mySeparatorVisibilityMap;
		std::vector<shared_ptr<ZLOptionView> > myViews;
		int myWidgetCounter;
	};

	Toolbar myToolbar;
};

#endif /* __ZLGTKAPPLICATIONWINDOW_H__ */
