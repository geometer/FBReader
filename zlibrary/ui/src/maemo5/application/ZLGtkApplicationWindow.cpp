/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <ZLPopupData.h>

#include "../message/ZLMaemoMessage.h"
#include "../view/ZLGtkViewWidget.h"
#include "../../gtk/util/ZLGtkKeyUtil.h"
#include "../../gtk/util/ZLGtkSignalUtil.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../dialogs/ZLGtkOptionsDialog.h"
#include "../optionView/ZLGtkOptionViewHolder.h"
#include "../../../../core/src/dialogs/ZLOptionView.h"

#include "ZLGtkApplicationWindow.h"

void ZLGtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = GTK_WINDOW((new ZLGtkApplicationWindow(application))->getMainWindow());
	myIsInitialized = true;
}

static bool acceptAction() {
	return
		ZLGtkDialogManager::isInitialized() &&
		!((ZLGtkDialogManager&)ZLGtkDialogManager::Instance()).isWaiting();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow*)data)->application().closeView();
	}
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkViewWidget*)data)->doPaint();
	}
}

static void menuActionSlot(GtkWidget*, gpointer data) {
	if (acceptAction()) {
		((ZLApplication::Action*)data)->checkAndRun();
	}
}

static bool handleKeyPress(GtkWidget*, GdkEventKey *key, gpointer data) {
	if (acceptAction()) {
		return ((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key, false);
	}
	return true;
}

static bool handleKeyRelease(GtkWidget*, GdkEventKey *key, gpointer data) {
	if (acceptAction()) {
		return ((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key, true);
	}
	return true;
}

static void mousePressed(GtkWidget *area, GdkEventButton *event, gpointer data) {
	gtk_window_set_focus(GTK_WINDOW(gtk_widget_get_toplevel(area)), area);
	if (acceptAction()) {
		((ZLGtkViewWidget*)data)->onMousePressed(event);
	}
}

static void mouseReleased(GtkWidget*, GdkEventButton *event, gpointer data) {
	if (acceptAction()) {
		((ZLGtkViewWidget*)data)->onMouseReleased(event);
	}
}

static void mouseMoved(GtkWidget*, GdkEventMotion *event, gpointer data) {
	if (acceptAction()) {
		((ZLGtkViewWidget*)data)->onMouseMoved(event);
	}
}

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	KeyActionOnReleaseNotOnPressOption(ZLCategoryKey::CONFIG, "KeyAction", "OnRelease", false),
	myFullScreen(false) {
	myProgram = HILDON_PROGRAM(hildon_program_get_instance());
	g_set_application_name("");

	myWindow = HILDON_WINDOW(hildon_stackable_window_new());

	((ZLMaemoCommunicationManager&)ZLCommunicationManager::Instance()).init();

	myMenu = HILDON_APP_MENU(hildon_app_menu_new());
	hildon_window_set_app_menu(myWindow, myMenu);

	hildon_program_add_window(myProgram, myWindow);
	gtk_widget_show_all(GTK_WIDGET(myWindow));

	myViewWidget = 0;

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "key_press_event", GTK_SIGNAL_FUNC(handleKeyPress), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "key_release_event", GTK_SIGNAL_FUNC(handleKeyRelease), this);
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	((ZLGtkDialogManager&)ZLGtkDialogManager::Instance()).setMainWindow(0);
	((ZLMaemoCommunicationManager&)ZLCommunicationManager::Instance()).shutdown();
}

void ZLGtkApplicationWindow::init() {
	ZLApplicationWindow::init();
	gtk_main_iteration();
	usleep(5000);
	gtk_main_iteration();
	usleep(5000);
	gtk_main_iteration();
}

void ZLGtkApplicationWindow::initMenu() {
	MenuBuilder(*this).processMenu(application());
}

ZLGtkApplicationWindow::MenuBuilder::MenuBuilder(ZLGtkApplicationWindow &window) : myWindow(window) {
	//myMenuStack.push(myWindow.myMenu);
}

void ZLGtkApplicationWindow::MenuBuilder::processSubmenuBeforeItems(ZLMenubar::Submenu &submenu) {
  /*
	GtkMenuItem *gtkItem = GTK_MENU_ITEM(gtk_menu_item_new_with_label(submenu.menuName().c_str()));
	GtkMenu *gtkSubmenu = GTK_MENU(gtk_menu_new());
	gtk_menu_item_set_submenu(gtkItem, GTK_WIDGET(gtkSubmenu));
	gtk_menu_shell_append(GTK_MENU_SHELL(myMenuStack.top()), GTK_WIDGET(gtkItem));
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
	myMenuStack.push(gtkSubmenu);
	myWindow.mySubmenuItems.push_back(gtkItem);
  */
}

void ZLGtkApplicationWindow::MenuBuilder::processSubmenuAfterItems(ZLMenubar::Submenu&) {
	//myMenuStack.pop();
}

void ZLGtkApplicationWindow::MenuBuilder::processItem(ZLMenubar::PlainItem &item) {
	GtkWidget *gtkItem = gtk_button_new_with_label(item.name().c_str());
	const std::string &id = item.actionId();
	shared_ptr<ZLApplication::Action> action = myWindow.application().action(id);
	if (!action.isNull()) {
		ZLGtkSignalUtil::connectSignalAfter(GTK_OBJECT(gtkItem), "clicked", G_CALLBACK(menuActionSlot), &*action);
	}
	myWindow.myMenuItems[id] = gtkItem;
	hildon_app_menu_append(myWindow.myMenu, GTK_BUTTON(gtkItem));
	gtk_widget_show_all(GTK_WIDGET(gtkItem));
}

void ZLGtkApplicationWindow::MenuBuilder::processSepartor(ZLMenubar::Separator&) {
}

bool ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event, bool isKeyRelease) {
	const std::string &keyName = ZLGtkKeyUtil::keyName(event);
	if ((myViewWidget != 0) && (KeyActionOnReleaseNotOnPressOption.value() == isKeyRelease)) {
		application().doActionByKey(keyName);
	}
	return keyName == "<Escape>";
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	if (myFullScreen) {
		gtk_window_fullscreen(GTK_WINDOW(myWindow));
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(GTK_WINDOW(myWindow));
	}
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLGtkApplicationWindow::close() {
	ZLGtkSignalUtil::removeAllSignals();
	gtk_main_quit();
}

static bool itemActivated(GtkWidget *menuItem, gpointer data) {
	GtkMenu *menu = GTK_MENU(gtk_widget_get_parent(menuItem));
	GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
	int index = g_list_index(children, (gconstpointer)menuItem);
	((ZLPopupData*)data)->run(index);
	return true;
}

void ZLGtkApplicationWindow::updatePopupData(GtkMenuToolButton*, shared_ptr<ZLPopupData>) {
}

void ZLGtkApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr) {
}

void ZLGtkApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr, bool, bool) {
}

void ZLGtkApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem&) {
}

void ZLGtkApplicationWindow::present() {
	gtk_window_present(GTK_WINDOW(myWindow));
}

void ZLGtkApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();

/*
	for (std::map<std::string,GtkMenuItem*>::iterator it = myMenuItems.begin(); it != myMenuItems.end(); it++) {
		const std::string &id = it->first;
		GtkWidget *gtkItem = GTK_WIDGET(it->second);
		if (application().isActionVisible(id)) {
			gtk_widget_show(gtkItem);
		} else {
			gtk_widget_hide(gtkItem);
		}
		bool alreadyEnabled = (GTK_WIDGET_STATE(gtkItem) & GTK_STATE_INSENSITIVE) == 0;
		if (application().isActionEnabled(id) != alreadyEnabled) {
			gtk_widget_set_sensitive(gtkItem, !alreadyEnabled);
		}
	}
	for (std::vector<GtkMenuItem*>::reverse_iterator rit = mySubmenuItems.rbegin(); rit != mySubmenuItems.rend(); ++rit) {
		bool isVisible = false;
		bool isEnabled = false;
		GtkMenu *menu = GTK_MENU(gtk_menu_item_get_submenu(*rit));
		GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
		if (children != 0) {
			for (GList *ptr =  g_list_first(children); ptr != 0; ptr = g_list_next(ptr)) {
				GtkMenuItem *item = GTK_MENU_ITEM(ptr->data);
				if (GTK_WIDGET_VISIBLE(item)) {
					isVisible = true;
					isEnabled = (GTK_WIDGET_STATE(item) & GTK_STATE_INSENSITIVE) == 0;
					if (isEnabled) {
						break;
					}
				}
			}
		}
		if (isEnabled != ((GTK_WIDGET_STATE(*rit) & GTK_STATE_INSENSITIVE) == 0)) {
			gtk_widget_set_sensitive(GTK_WIDGET(*rit), isEnabled);
		}
		if (isVisible) {
			gtk_widget_show(GTK_WIDGET(*rit));
		} else {
			gtk_widget_hide(GTK_WIDGET(*rit));
		}
	}
  */
}

void ZLGtkApplicationWindow::processAllEvents() {
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}

ZLViewWidget *ZLGtkApplicationWindow::createViewWidget() {
	myViewWidget = new ZLGtkViewWidget(&application(), (ZLView::Angle)application().AngleStateOption.value());
	GtkWidget *area = myViewWidget->area();
	gtk_container_add(GTK_CONTAINER(myWindow), myViewWidget->areaWithScrollbars());
	GtkObject *areaObject = GTK_OBJECT(area);
	ZLGtkSignalUtil::connectSignal(areaObject, "expose_event", GTK_SIGNAL_FUNC(repaint), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "button_press_event", GTK_SIGNAL_FUNC(mousePressed), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "button_release_event", GTK_SIGNAL_FUNC(mouseReleased), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "motion_notify_event", GTK_SIGNAL_FUNC(mouseMoved), myViewWidget);
	gtk_widget_show(GTK_WIDGET(myWindow));

	return myViewWidget;
}

void ZLGtkApplicationWindow::grabAllKeys(bool) {
}

void ZLGtkApplicationWindow::setFocusToMainWidget() {
	gtk_window_set_focus(GTK_WINDOW(myWindow), myViewWidget->area());
}
