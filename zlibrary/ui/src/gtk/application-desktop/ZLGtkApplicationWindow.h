/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <gtk/gtktoolitem.h>
#include <gtk/gtkmenutoolbutton.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkhandlebox.h>

#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"
#include "../../../../core/src/application/ZLToolbar.h"

class ZLGtkViewWidget;

class ZLGtkApplicationWindow : public ZLDesktopApplicationWindow { 

public:
	ZLGtkApplicationWindow(ZLApplication *application);
	~ZLGtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void refresh();
	void processAllEvents();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption) { gtk_window_set_title (myMainWindow, caption.c_str ()); }

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

	void readPosition();
	void setPosition();

public:
	bool handleKeyEventSlot(GdkEventKey *event);
	void handleScrollEventSlot(GdkEventScroll *event);
	void onGtkButtonPress(GtkToolItem *gtkButton);

	GtkWindow *getMainWindow() { return myMainWindow; }
	void setFocusToMainWidget();

public:
	class GtkEntryParameter : public VisualParameter {

	public:
		GtkEntryParameter(ZLGtkApplicationWindow &window, const ZLToolbar::ParameterItem &item);
		bool onKeyPressed(const std::string &keyName);
		void onValueChanged();
		GtkToolItem *createToolItem();

	private:
		std::string internalValue() const;
		void internalSetValue(const std::string &value);
		void setValueList(const std::vector<std::string> &values);

	private:
		ZLGtkApplicationWindow &myWindow;
		const ZLToolbar::ParameterItem &myItem;
		GtkWidget *myWidget;
		GtkEntry *myEntry;
	};

private:
	GtkWindow *myMainWindow;
	ZLGtkViewWidget *myViewWidget;

	GtkWidget *myVBox;

	GdkCursor *myHyperlinkCursor;
	bool myHyperlinkCursorIsUsed;

	class Toolbar {

	public:
		Toolbar(ZLGtkApplicationWindow *window);

		GtkWidget *toolbarWidget() const;

		void addToolbarItem(ZLToolbar::ItemPtr item);
		void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
		void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);
		ZLToolbar::AbstractButtonItem &buttonItemByWidget(GtkToolItem *gtkButton);

	private:
		GtkToolItem *createGtkToolButton(const ZLToolbar::AbstractButtonItem &button);
		void updatePopupData(GtkMenuToolButton *button, shared_ptr<ZLPopupData> data);

	private:
		ZLGtkApplicationWindow *myWindow;
		GtkToolbar *myGtkToolbar;
		std::map<const ZLToolbar::Item*,GtkToolItem*> myAbstractToGtk;
		std::map<GtkToolItem*,ZLToolbar::ItemPtr> myGtkToAbstract;
		std::map<GtkToolItem*,size_t> myPopupIdMap;

	friend class ZLGtkApplicationWindow;
	};

	Toolbar myWindowToolbar;
	Toolbar myFullscreenToolbar;

	Toolbar &toolbar(ToolbarType type) {
		return (type == WINDOW_TOOLBAR) ? myWindowToolbar : myFullscreenToolbar;
	}

	GtkHandleBox *myHandleBox;
};

#endif /* __ZLGTKAPPLICATIONWINDOW_H__ */
