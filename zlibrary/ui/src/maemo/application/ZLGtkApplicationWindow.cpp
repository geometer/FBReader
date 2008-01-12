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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <ZLibrary.h>
#include <ZLOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>

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
		((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key, false);
	}
	return false;
}

static bool handleKeyRelease(GtkWidget*, GdkEventKey *key, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key, true);
	}
	return false;
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

	myWindow = HILDON_WINDOW(hildon_window_new());

	((ZLMaemoCommunicationManager&)ZLCommunicationManager::instance()).init();

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_toolbar_set_show_arrow(myToolbar, false);
	gtk_toolbar_set_orientation(myToolbar, GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	myMenu = GTK_MENU(gtk_menu_new());
	hildon_window_set_menu(myWindow, myMenu);
	gtk_widget_show_all(GTK_WIDGET(myMenu));

	hildon_window_add_toolbar(myWindow, myToolbar);
	hildon_program_add_window(myProgram, myWindow);
	gtk_widget_show_all(GTK_WIDGET(myWindow));

	myViewWidget = 0;

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "key_press_event", GTK_SIGNAL_FUNC(handleKeyPress), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myWindow), "key_release_event", GTK_SIGNAL_FUNC(handleKeyRelease), this);
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	((ZLGtkDialogManager&)ZLGtkDialogManager::instance()).setMainWindow(0);
	for (std::map<const ZLApplication::Toolbar::ButtonItem*,ToolbarButton*>::iterator it = myToolbarButtons.begin(); it != myToolbarButtons.end(); ++it) {
		delete it->second;
	}

	((ZLMaemoCommunicationManager&)ZLCommunicationManager::instance()).shutdown();
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

void ZLGtkApplicationWindow::MenuBuilder::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
	myMenuStack.pop();
}

void ZLGtkApplicationWindow::MenuBuilder::processItem(ZLApplication::Menubar::PlainItem &item) {
	GtkMenuItem *gtkItem = GTK_MENU_ITEM(gtk_menu_item_new_with_label(item.name().c_str()));
	const std::string &id = item.actionId();
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

void ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event, bool isKeyRelease) {
	if ((myViewWidget != 0) && (KeyActionOnReleaseNotOnPressOption.value() == isKeyRelease)) {
		application().doActionByKey(ZLGtkKeyUtil::keyName(event));
	}
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	if (myFullScreen) {
		gtk_window_fullscreen(GTK_WINDOW(myWindow));
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(GTK_WINDOW(myWindow));
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

class ZLGtkToolItemWrapper : public ZLGtkOptionViewHolder {

public:
	ZLGtkToolItemWrapper(GtkToolItem *item);

private:
	void attachWidget(ZLOptionView &view, GtkWidget *widget);
	void attachWidgets(ZLOptionView &view, GtkWidget *widget0, int weight0, GtkWidget *widget1, int weight1);

private:
	GtkToolItem *myItem;
};

ZLGtkToolItemWrapper::ZLGtkToolItemWrapper(GtkToolItem *item) : myItem(item) {
}

void ZLGtkToolItemWrapper::attachWidget(ZLOptionView&, GtkWidget *widget) {
	gtk_container_set_border_width(GTK_CONTAINER(myItem), 2);
	gtk_container_add(GTK_CONTAINER(myItem), widget);
}

void ZLGtkToolItemWrapper::attachWidgets(ZLOptionView&, GtkWidget *widget0, int weight0, GtkWidget *widget1, int weight1) {
	// TODO: implement
}

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	GtkToolItem *gtkItem = 0;
	switch (item->type()) {
		case ZLApplication::Toolbar::Item::OPTION_ENTRY:
			{
				shared_ptr<ZLOptionEntry> entry = ((const ZLApplication::Toolbar::OptionEntryItem&)*item).entry();
				gtkItem = gtk_tool_item_new();
				ZLGtkToolItemWrapper wrapper(gtkItem);
				ZLOptionView *view = wrapper.createViewByEntry("", "", entry);
				if (view != 0) {
					gtk_tool_item_set_homogeneous(gtkItem, false);
					gtk_tool_item_set_expand(gtkItem, false);
					GTK_WIDGET_UNSET_FLAGS(gtkItem, GTK_CAN_FOCUS);
					myViews.push_back(view);
					entry->setVisible(true);
				} else {
					// TODO: remove *gtkItem
					gtkItem = 0;
				}
			}
			break;
		case ZLApplication::Toolbar::Item::BUTTON:
			{
				ZLApplication::Toolbar::ButtonItem &buttonItem = (ZLApplication::Toolbar::ButtonItem&)*item;
      
				ToolbarButton *toolbarButton = new ToolbarButton(buttonItem, *this);
				gtkItem = toolbarButton->toolItem();
				myToolbarButtons[&buttonItem] = toolbarButton;
			}
			break;
		case ZLApplication::Toolbar::Item::SEPARATOR:
			{
				gtkItem = gtk_separator_tool_item_new();
				gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(gtkItem), false);
			}
			break;
	}
	if (gtkItem != 0) {
		gtk_toolbar_insert(myToolbar, gtkItem, -1);
		myToolItems[item] = gtkItem;
		gtk_widget_show_all(GTK_WIDGET(gtkItem));
	}
}

void ZLGtkApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	std::map<ZLApplication::Toolbar::ItemPtr,GtkToolItem*>::iterator it = myToolItems.find(item);
	if (it != myToolItems.end()) {
		GtkToolItem *toolItem = it->second;
		gtk_tool_item_set_visible_horizontal(toolItem, visible);
		/*
		 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
		 * does something strange if WIDGET is already insensitive.
		 */
		bool alreadyEnabled = GTK_WIDGET_STATE(toolItem) != GTK_STATE_INSENSITIVE;
		if (enabled != alreadyEnabled) {
			gtk_widget_set_sensitive(GTK_WIDGET(toolItem), enabled);
		}
	}
}

void ZLGtkApplicationWindow::present() {
	gtk_window_present(GTK_WINDOW(myWindow));
}

void ZLGtkApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();

	for (std::map<std::string,GtkMenuItem*>::iterator it = myMenuItems.begin(); it != myMenuItems.end(); it++) {
		const std::string &id = it->first;
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
	myViewWidget = new ZLGtkViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	GtkWidget *area = myViewWidget->area();
	gtk_container_add(GTK_CONTAINER(myWindow), area);
	GtkObject *areaObject = GTK_OBJECT(area);
	ZLGtkSignalUtil::connectSignal(areaObject, "expose_event", GTK_SIGNAL_FUNC(repaint), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "button_press_event", GTK_SIGNAL_FUNC(mousePressed), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "button_release_event", GTK_SIGNAL_FUNC(mouseReleased), myViewWidget);
	ZLGtkSignalUtil::connectSignal(areaObject, "motion_notify_event", GTK_SIGNAL_FUNC(mouseMoved), myViewWidget);
	gtk_widget_show_all(GTK_WIDGET(myWindow));

	ZLGtkOptionsDialog::addMaemoBuilder(this);

	return myViewWidget;
}

void ZLGtkApplicationWindow::grabAllKeys(bool) {
}

static void onGtkButtonPress(GtkWidget*, GdkEventButton*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow::ToolbarButton*)data)->press(true);
	}
}

static void onGtkButtonRelease(GtkWidget*, GdkEventButton*, gpointer data) {
	if (acceptAction()) {
		((ZLGtkApplicationWindow::ToolbarButton*)data)->press(false);
	}
}

ZLGtkApplicationWindow::ToolbarButton::ToolbarButton(ZLApplication::Toolbar::ButtonItem &buttonItem, ZLGtkApplicationWindow &window) : myButtonItem(buttonItem), myWindow(window) {
	myAction = myWindow.application().action(buttonItem.actionId());

	GdkPixbuf *filePixbuf = gdk_pixbuf_new_from_file((ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + buttonItem.iconName() + ".png").c_str(), 0);

	const int width = gdk_pixbuf_get_width(filePixbuf);
	const int height = gdk_pixbuf_get_height(filePixbuf);
	const int border = 4;
	const int line = 2;
	const GdkColorspace colorspace = gdk_pixbuf_get_colorspace(filePixbuf);
	const bool hasAlpha = gdk_pixbuf_get_has_alpha(filePixbuf);
	const int bitsPerSample = gdk_pixbuf_get_bits_per_sample(filePixbuf);

	const int w = width + 2 * border;
	const int h = height + 2 * border;
	GdkPixbuf *buttonPixbuf = gdk_pixbuf_new(colorspace, hasAlpha, bitsPerSample, w, h);
	gdk_pixbuf_fill(buttonPixbuf, 0);
	gdk_pixbuf_copy_area(filePixbuf, 0, 0, width, height, buttonPixbuf, border, border);
	myCurrentImage = GTK_IMAGE(gtk_image_new_from_pixbuf(buttonPixbuf));
	myReleasedImage = GTK_IMAGE(gtk_image_new_from_pixbuf(buttonPixbuf));

	GdkPixbuf *pressedButtonPixbuf = gdk_pixbuf_copy(buttonPixbuf);
	GdkPixbuf *top = gdk_pixbuf_new_subpixbuf(pressedButtonPixbuf, 0, 0, w, line);
	GdkPixbuf *bottom = gdk_pixbuf_new_subpixbuf(pressedButtonPixbuf, 0, h - line, w, line);
	GdkPixbuf *left = gdk_pixbuf_new_subpixbuf(pressedButtonPixbuf, 0, 0, line, h);
	GdkPixbuf *right = gdk_pixbuf_new_subpixbuf(pressedButtonPixbuf, w - line, 0, line, h);
	gdk_pixbuf_fill(top, 0x00007FFF);
	gdk_pixbuf_fill(bottom, 0x00007FFF);
	gdk_pixbuf_fill(left, 0x00007FFF);
	gdk_pixbuf_fill(right, 0x00007FFF);
	gdk_pixbuf_copy_area(filePixbuf, 0, 0, width, height, pressedButtonPixbuf, border, border);
	myPressedImage = GTK_IMAGE(gtk_image_new_from_pixbuf(pressedButtonPixbuf));

	gdk_pixbuf_unref(filePixbuf);
	gdk_pixbuf_unref(buttonPixbuf);
	gdk_pixbuf_unref(pressedButtonPixbuf);

	myEventBox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(myEventBox), GTK_WIDGET(myCurrentImage));
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myEventBox), "button_press_event", GTK_SIGNAL_FUNC(onGtkButtonPress), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myEventBox), "button_release_event", GTK_SIGNAL_FUNC(onGtkButtonRelease), this);

	myToolItem = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(myToolItem), myEventBox);
	gtk_tool_item_set_homogeneous(myToolItem, false);
	gtk_tool_item_set_expand(myToolItem, false);
	GTK_WIDGET_UNSET_FLAGS(myToolItem, GTK_CAN_FOCUS);
}

void ZLGtkApplicationWindow::ToolbarButton::forcePress(bool state) {
	gtk_image_set_from_pixbuf(myCurrentImage, gtk_image_get_pixbuf(state ? myPressedImage : myReleasedImage));
}

void ZLGtkApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	myToolbarButtons[&button]->forcePress(button.isPressed());
}

void ZLGtkApplicationWindow::ToolbarButton::press(bool state) {
	if (!myButtonItem.isToggleButton()) {
		forcePress(state);
		if (state) {
			return;
		}
	}
	myWindow.onButtonPress(myButtonItem);
}

void ZLGtkApplicationWindow::buildTabs(ZLOptionsDialog &dialog) {
	ZLDialogContent &tab = dialog.createTab(ZLResourceKey("Maemo"));
	tab.addOption(
		ZLResourceKey("keyActionOnRelease"),
		KeyActionOnReleaseNotOnPressOption
	);
	tab.addOption(
		ZLResourceKey("minStylusPressure"),
		new ZLSimpleSpinOptionEntry(myViewWidget->MinPressureOption, 1)
	);
	tab.addOption(
		ZLResourceKey("maxStylusPressure"),
		new ZLSimpleSpinOptionEntry(myViewWidget->MaxPressureOption, 1)
	);
}
