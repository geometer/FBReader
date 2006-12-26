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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "../view/ZLGtkViewWidget.h"
#include "../../gtk/util/ZLGtkKeyUtil.h"
#include "../../gtk/util/ZLGtkSignalUtil.h"
#include "../dialogs/ZLGtkDialogManager.h"

#include "ZLGtkApplicationWindow.h"

void ZLGtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = GTK_WINDOW((new ZLGtkApplicationWindow(application))->getMainWindow());
	myIsInitialized = true;
}

static bool acceptAction() {
	return
		ZLGtkDialogManager::isInitialized() &&
		!((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).isWaiting();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow*)data)->application().closeView();
	}
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow*)data)->application().refreshWindow();
	}
}

static void actionSlot(GtkWidget*, GdkEventButton*, gpointer data) {
	if (acceptAction()) {
		((ZLApplication::Action*)data)->checkAndRun();
	}
}

static void menuActionSlot(GtkWidget *, gpointer data) {
	if (acceptAction()) {
		((ZLApplication::Action*)data)->checkAndRun();
	}
}

static bool handleKey(GtkWidget*, GdkEventKey *key, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key);
	}
	return false;
}

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
	if (acceptAction()) {
		((ZLGtkViewWidget*)data)->onMousePressed(event);
	}
}

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) : ZLApplicationWindow(application) {
	myApp = HILDON_APP(hildon_app_new());
	hildon_app_set_title(myApp, ZLApplication::ApplicationName().c_str());
	hildon_app_set_two_part_title(myApp, FALSE);

	osso_initialize(ZLApplication::ApplicationName().c_str(), "0.0", true, 0);

	myAppView = HILDON_APPVIEW(hildon_appview_new(0));

	myMenu = GTK_MENU(hildon_appview_get_menu(myAppView));

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_toolbar_set_show_arrow(myToolbar, false);
	gtk_toolbar_set_orientation(myToolbar, GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	gtk_widget_show_all(GTK_WIDGET(myMenu));

	hildon_appview_set_toolbar(myAppView, myToolbar);

	hildon_app_set_appview(myApp, myAppView);

	gtk_widget_show_all(GTK_WIDGET(myApp));

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myApp), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myApp), "key_press_event", GTK_SIGNAL_FUNC(handleKey), this);

	myFullScreen = false;
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).setMainWindow(0);
}

void ZLGtkApplicationWindow::initMenu() {
	MenuBuilder(*this).processMenu(application().menubar());
}

ZLGtkApplicationWindow::MenuBuilder::MenuBuilder(ZLGtkApplicationWindow &window) : myWindow(window) {
	myMenuStack.push(myWindow.myMenu);
}

void ZLGtkApplicationWindow::MenuBuilder::processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu) {
	GtkMenuItem *gtkItem = GTK_MENU_ITEM(gtk_menu_item_new_with_label(submenu.menuName().c_str()));
	GtkMenu *gtkSubmenu = GTK_MENU(gtk_menu_new());
	gtk_menu_item_set_submenu(gtkItem, GTK_WIDGET(gtkSubmenu));
	gtk_menu_shell_append(GTK_MENU_SHELL(myMenuStack.top()), GTK_WIDGET(gtkItem));
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
	myMenuStack.push(gtkSubmenu);
}

void ZLGtkApplicationWindow::MenuBuilder::processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu) {
	myMenuStack.pop();
}

void ZLGtkApplicationWindow::MenuBuilder::processItem(ZLApplication::Menubar::PlainItem &item) {
	GtkMenuItem *gtkItem = GTK_MENU_ITEM(gtk_menu_item_new_with_label(item.name().c_str()));
	const int id = item.actionId();
	shared_ptr<ZLApplication::Action> action = myWindow.application().action(id);
	if (!action.isNull()) {
		ZLGtkSignalUtil::connectSignal(GTK_OBJECT(gtkItem), "activate", GTK_SIGNAL_FUNC(menuActionSlot), &*action);
	}
	myWindow.myMenuItems[id] = gtkItem;
	gtk_menu_shell_append(GTK_MENU_SHELL(myMenuStack.top()), GTK_WIDGET(gtkItem));
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
}

void ZLGtkApplicationWindow::MenuBuilder::processSepartor(ZLApplication::Menubar::Separator&) {
	GtkMenuItem *gtkItem = GTK_MENU_ITEM(gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(myMenuStack.top()), GTK_WIDGET(gtkItem));
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
}

void ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(ZLGtkKeyUtil::keyName(event));
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	hildon_appview_set_fullscreen(myAppView, myFullScreen);
	if (myFullScreen) {
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLGtkApplicationWindow::close() {
	ZLGtkSignalUtil::removeAllSignals();
	gtk_main_quit();
}

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	GtkToolItem *gtkItem;
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + ZLApplication::ApplicationName() + ZLApplication::PathDelimiter + buttonItem.iconName() + ".png").c_str());
		gtkItem = gtk_tool_item_new();
		GtkWidget *ebox = gtk_event_box_new();

		gtk_container_add(GTK_CONTAINER(ebox), image);
		gtk_container_add(GTK_CONTAINER(gtkItem), ebox);

		gtk_tool_item_set_homogeneous(gtkItem, false);
		gtk_tool_item_set_expand(gtkItem, false);

		GTK_WIDGET_UNSET_FLAGS(gtkItem, GTK_CAN_FOCUS);
		shared_ptr<ZLApplication::Action> action = application().action(buttonItem.actionId());
		if (!action.isNull()) {
			ZLGtkSignalUtil::connectSignal(GTK_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), &*action);
		}
	} else {
		gtkItem = gtk_separator_tool_item_new();
		gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(gtkItem), false);
	}
	gtk_toolbar_insert(myToolbar, gtkItem, -1);
	myButtons[item] = gtkItem;
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
}

void ZLGtkApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	bool enableToolbarSpace = false;
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		GtkToolItem *toolItem = myButtons[*it];
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			int id = button.actionId();

			bool visible = application().isActionVisible(id);
			bool enabled = application().isActionEnabled(id);

			if (toolItem != 0) {
				gtk_tool_item_set_visible_horizontal(toolItem, visible);
				if (visible) {
					enableToolbarSpace = true;
				}
				/*
				 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
				 * does something strange if WIDGET is already insensitive.
				 */
				bool alreadyEnabled = GTK_WIDGET_STATE(toolItem) != GTK_STATE_INSENSITIVE;
				if (enabled != alreadyEnabled) {
					gtk_widget_set_sensitive(GTK_WIDGET(toolItem), enabled);
				}
			}
		} else {
			if (toolItem != 0) {
				gtk_tool_item_set_visible_horizontal(toolItem, enableToolbarSpace);
				enableToolbarSpace = false;
			}
		}
	}

	for (std::map<int,GtkMenuItem*>::iterator it = myMenuItems.begin(); it != myMenuItems.end(); it++) {
		int id = it->first;
		GtkWidget *gtkItem = GTK_WIDGET(it->second);
		if (application().isActionVisible(id)) {
			gtk_widget_show(gtkItem);
		} else {
			gtk_widget_hide(gtkItem);
		}
		bool alreadyEnabled = GTK_WIDGET_STATE(gtkItem) != GTK_STATE_INSENSITIVE;
		if (application().isActionEnabled(id) != alreadyEnabled) {
			gtk_widget_set_sensitive(gtkItem, !alreadyEnabled);
		}
	}
}

ZLViewWidget *ZLGtkApplicationWindow::createViewWidget() {
	ZLGtkViewWidget *viewWidget = new ZLGtkViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myAppView), viewWidget->area());
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), viewWidget);
	gtk_widget_show_all(GTK_WIDGET(myAppView));
	return viewWidget;
}

bool ZLGtkApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void ZLGtkApplicationWindow::grabAllKeys(bool) {
}

bool ZLGtkApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

bool ZLGtkApplicationWindow::isMousePresented() const {
	return false;
}

bool ZLGtkApplicationWindow::isKeyboardPresented() const {
	return false;
}
