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
#include <stack>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#if MAEMO_VERSION == 2
	#include <hildon-widgets/hildon-program.h>
#elif MAEMO_VERSION == 4
	#include <hildon/hildon-program.h>
#else
	#error Unknown MAEMO_VERSION
#endif

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/dialogs/ZLDialogContentBuilder.h"

class ZLGtkViewWidget;

class ZLGtkApplicationWindow : public ZLApplicationWindow, public ZLDialogContentBuilder { 

private:
	ZLBooleanOption KeyActionOnReleaseNotOnPressOption;

public:
	ZLGtkApplicationWindow(ZLApplication *application);
	~ZLGtkApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void init();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void buildTabs(ZLOptionsDialog &dialog);

	class MenuBuilder : public ZLApplication::MenuVisitor {

	public:
		MenuBuilder(ZLGtkApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		ZLGtkApplicationWindow &myWindow;
		std::stack<GtkMenu*> myMenuStack;
	};
	void initMenu();
	void refresh();
	void present();
	void close();

	void setCaption(const std::string &caption) { gtk_window_set_title(GTK_WINDOW(myWindow), caption.c_str()); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void grabAllKeys(bool grab);

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
	void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);

public:
	void handleKeyEventSlot(GdkEventKey *event, bool isKeyRelease);
	HildonWindow *getMainWindow() const { return myWindow; }

public:
	class ToolbarButton {

	public:
		void press(bool state);

	private:
		ToolbarButton(ZLApplication::Toolbar::ButtonItem &buttonItem, ZLGtkApplicationWindow &window);
		void forcePress(bool state);
		GtkToolItem *toolItem() const { return myToolItem; }

	private:
		ZLApplication::Toolbar::ButtonItem &myButtonItem;
		ZLGtkApplicationWindow &myWindow;
		shared_ptr<ZLApplication::Action> myAction;

		GtkToolItem *myToolItem;
		GtkWidget *myEventBox;
		GtkImage *myCurrentImage;
		GtkImage *myReleasedImage;
		GtkImage *myPressedImage;

	friend class ZLGtkApplicationWindow;
	};

private:
	HildonProgram *myProgram;
	HildonWindow *myWindow;
	GtkToolbar *myToolbar;
	GtkMenu *myMenu;
	ZLGtkViewWidget *myViewWidget;

	bool myFullScreen;

	std::map<ZLApplication::Toolbar::ItemPtr,GtkToolItem*> myToolItems;
	std::map<std::string,GtkMenuItem*> myMenuItems;
	std::map<const ZLApplication::Toolbar::ButtonItem*,ToolbarButton*> myToolbarButtons;
	std::vector<shared_ptr<ZLOptionView> > myViews;

friend class MenuBuilder;
friend class ToolbarButton;
};

#endif /* __ZLGTKAPPLICATIONWINDOW_H__ */
