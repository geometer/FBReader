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
#include <stack>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#if MAEMO_VERSION <= 3
	#include <hildon-widgets/hildon-program.h>
#elif MAEMO_VERSION <= 5
	#include <hildon/hildon-program.h>
#else
	#error Unknown MAEMO_VERSION
#endif

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"
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
	GtkToolItem *createGtkToolButton(const ZLToolbar::AbstractButtonItem &button);
	void updatePopupData(GtkMenuToolButton *button, shared_ptr<ZLPopupData> data);
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void buildTabs(ZLOptionsDialog &dialog);

	class MenuBuilder : public ZLMenuVisitor {

	public:
		MenuBuilder(ZLGtkApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLMenubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLMenubar::Submenu &submenu);
		void processItem(ZLMenubar::PlainItem &item);
		void processSepartor(ZLMenubar::Separator &separator);

	private:
		ZLGtkApplicationWindow &myWindow;
	};
	void initMenu();
	void refresh();
	void processAllEvents();
	void present();
	void close();

	void setCaption(const std::string &caption) { gtk_window_set_title(GTK_WINDOW(myWindow), caption.c_str()); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void grabAllKeys(bool grab);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

public:
	bool handleKeyEventSlot(GdkEventKey *event, bool isKeyRelease);
	void onGtkButtonPress(GtkToolItem *gtkButton);
	HildonWindow *getMainWindow() const { return myWindow; }
	void setFocusToMainWidget();

public:
	class GtkEntryParameter : public VisualParameter {

	public:
		GtkEntryParameter(ZLGtkApplicationWindow &window, const ZLToolbar::ParameterItem &item);
		void onKeyPressed(const std::string &keyName);
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
	HildonProgram *myProgram;
	HildonWindow *myWindow;
	GtkToolbar *myToolbar;
	HildonAppMenu *myMenu;
	ZLGtkViewWidget *myViewWidget;

	bool myFullScreen;

	std::map<const ZLToolbar::Item*,GtkToolItem*> myAbstractToGtk;
	std::map<GtkToolItem*,ZLToolbar::ItemPtr> myGtkToAbstract;
	std::map<GtkToolItem*,size_t> myPopupIdMap;
	std::map<std::string,GtkWidget*> myMenuItems;
	std::vector<GtkMenuItem*> mySubmenuItems;

friend class MenuBuilder;
};

#endif /* __ZLGTKAPPLICATIONWINDOW_H__ */
