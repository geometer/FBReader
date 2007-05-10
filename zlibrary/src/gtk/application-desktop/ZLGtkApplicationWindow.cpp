/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLGtkApplicationWindow.h"

#include "../util/ZLGtkKeyUtil.h"
#include "../util/ZLGtkSignalUtil.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../view/ZLGtkViewWidget.h"

void ZLGtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = (new ZLGtkApplicationWindow(application))->getMainWindow();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLGtkApplicationWindow*)data)->application().closeView();
	return true;
}

static void onButtonClicked(GtkWidget *button, gpointer data) {
	((ZLGtkApplicationWindow*)data)->onGtkButtonPress(button);
}

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, gpointer data) {
	((ZLGtkApplicationWindow*)data)->handleScrollEventSlot(event);
}

static const std::string OPTIONS = "Options";

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myHyperlinkCursor(0),
	myHyperlinkCursorIsUsed(false) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	const std::string iconFileName = ZLApplication::ImageDirectory() + ZLApplication::FileNameDelimiter + ZLApplication::ApplicationName() + ".png";
	gtk_window_set_icon(myMainWindow, gdk_pixbuf_new_from_file(iconFileName.c_str(), 0));
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_box_pack_start(GTK_BOX(myVBox), GTK_WIDGET(myToolbar), false, false, 0);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_window_move(myMainWindow, myXOption.value(), myYOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "scroll_event", G_CALLBACK(handleScrollEvent), this);
}

void ZLGtkApplicationWindow::init() {
	ZLDesktopApplicationWindow::init();
	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			setFullscreen(true);
			break;
		case MAXIMIZED:
			gtk_window_maximize(myMainWindow);
			break;
	}
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	switch (gdk_window_get_state(GTK_WIDGET(myMainWindow)->window)) {
		case GDK_WINDOW_STATE_MAXIMIZED:
			myWindowStateOption.setValue(MAXIMIZED);
			break;
		case GDK_WINDOW_STATE_FULLSCREEN:
			myWindowStateOption.setValue(FULLSCREEN);
			break;
		default:
		{
			myWindowStateOption.setValue(NORMAL);
			int x, y, width, height;
			gtk_window_get_position(myMainWindow, &x, &y);
			gtk_window_get_size(myMainWindow, &width, &height);
			myXOption.setValue(x);
			myYOption.setValue(y);
			myWidthOption.setValue(width);
			myHeightOption.setValue(height);
		}
	}
}

void ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(ZLGtkKeyUtil::keyName(event));
}

void ZLGtkApplicationWindow::handleScrollEventSlot(GdkEventScroll *event) {
	switch (event->direction) {
		case GDK_SCROLL_UP:
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
			break;
		case GDK_SCROLL_DOWN:
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
			break;
		default:
			break;
	}
}

void ZLGtkApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	GtkToggleButton *gtkButton = GTK_TOGGLE_BUTTON(myButtonToWidget[&(ZLApplication::Toolbar::Item&)button]);
	const bool isPressed = button.isPressed();
	if (gtk_toggle_button_get_active(gtkButton) != isPressed) {
		gtk_toggle_button_set_active(gtkButton, isPressed);
	}
}

void ZLGtkApplicationWindow::onGtkButtonPress(GtkWidget *gtkButton) {
	onButtonPress((ZLApplication::Toolbar::ButtonItem&)*myWidgetToButton[gtkButton]);
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == isFullscreen()) {
		return;
	}

	if (fullscreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return
		gdk_window_get_state(GTK_WIDGET(myMainWindow)->window) ==
		GDK_WINDOW_STATE_FULLSCREEN;
}

static const int VISIBLE_SEPARATOR = 1 << 16;

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		static std::string imagePrefix = ZLApplication::ApplicationImageDirectory() + ZLApplication::FileNameDelimiter;
		GtkWidget *image = gtk_image_new_from_file((imagePrefix + buttonItem.iconName() + ".png").c_str());
		GtkWidget *button = buttonItem.isToggleButton() ? gtk_toggle_button_new() : gtk_button_new();
		gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
		GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
		gtk_container_add(GTK_CONTAINER(button), image);
		//gtk_container_add(GTK_CONTAINER(myToolbar), button);
		gtk_toolbar_append_widget(myToolbar, button, buttonItem.tooltip().c_str(), 0);
		ZLGtkSignalUtil::connectSignal(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(onButtonClicked), this);
		myButtonToWidget[&*item] = button;
		myWidgetToButton[button] = item;
		gtk_widget_show_all(button);
	} else {
		int index = myButtonToWidget.size() + mySeparatorMap.size();
		mySeparatorMap[item] = index;
	}
}

void ZLGtkApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	GtkWidget *gtkButton = myButtonToWidget[&*item];
	if (gtkButton != 0) {
		if (visible) {
			gtk_widget_show(gtkButton);
		} else {
			gtk_widget_hide(gtkButton);
		}
		/*
		 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
		 * does something strange if WIDGET is already insensitive.
		 */
		bool alreadyEnabled = GTK_WIDGET_STATE(gtkButton) != GTK_STATE_INSENSITIVE;
		if (enabled != alreadyEnabled) {
			gtk_widget_set_sensitive(gtkButton, enabled);
		}
	} else {
		int &index = mySeparatorMap[item];
		if (visible) {
			if ((index & VISIBLE_SEPARATOR) == 0) {
				gtk_toolbar_insert_space(myToolbar, index);
				index |= VISIBLE_SEPARATOR;
			}
		} else {
			if ((index & VISIBLE_SEPARATOR) != 0) {
				index &= ~VISIBLE_SEPARATOR;
				gtk_toolbar_remove_space(myToolbar, index);
			}
		}
	}
}

void ZLGtkApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

ZLViewWidget *ZLGtkApplicationWindow::createViewWidget() {
	ZLGtkViewWidget *viewWidget = new ZLGtkViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myVBox), viewWidget->area());
	gtk_widget_show_all(myVBox);
	return viewWidget;
}

void ZLGtkApplicationWindow::close() {
	ZLGtkSignalUtil::removeAllSignals();
	gtk_main_quit();
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
	return true;
}

bool ZLGtkApplicationWindow::isKeyboardPresented() const {
	return true;
}

void ZLGtkApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	if (hyperlink == myHyperlinkCursorIsUsed) {
		return;
	}
	myHyperlinkCursorIsUsed = hyperlink;

	if (hyperlink) {
		if (myHyperlinkCursor == 0) {
			myHyperlinkCursor = gdk_cursor_new(GDK_HAND1);
		}
		gdk_window_set_cursor(GTK_WIDGET(myMainWindow)->window, myHyperlinkCursor);
	} else {
		gdk_window_set_cursor(GTK_WIDGET(myMainWindow)->window, 0);
	}
}
