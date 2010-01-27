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

#include <ZLibrary.h>

#include "ZLGtkApplicationWindow.h"

#include "../util/ZLGtkKeyUtil.h"
#include "../util/ZLGtkSignalUtil.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../view/ZLGtkViewWidget.h"

void ZLGtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = (new ZLGtkApplicationWindow(application))->getMainWindow();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, ZLGtkApplicationWindow *data) {
	data->application().closeView();
	return true;
}

static bool presentHandler(GtkWidget*, GdkEvent*, GtkWindow *data) {
	gtk_window_present(data);
	return false;
}

static bool handleKeyEvent(GtkWidget*, GdkEventKey *event, ZLGtkApplicationWindow *data) {
	return data->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, ZLGtkApplicationWindow *data) {
	data->handleScrollEventSlot(event);
}

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myViewWidget(0),
	myHyperlinkCursor(0),
	myHyperlinkCursorIsUsed(false),
	myWindowToolbar(this),
	myFullscreenToolbar(this),
	myHandleBox(0) {
	myMainWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	gtk_window_set_icon(myMainWindow, gdk_pixbuf_new_from_file(iconFileName.c_str(), 0));
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	if (hasFullscreenToolbar()) {
		myHandleBox = GTK_HANDLE_BOX(gtk_handle_box_new());
		gtk_toolbar_set_show_arrow(GTK_TOOLBAR(myFullscreenToolbar.toolbarWidget()), false);
		gtk_container_add(GTK_CONTAINER(myHandleBox), myFullscreenToolbar.toolbarWidget());
		gtk_box_pack_start(GTK_BOX(myVBox), GTK_WIDGET(myHandleBox), false, false, 0);
		ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myHandleBox), "event", GTK_SIGNAL_FUNC(presentHandler), myMainWindow);
	}

	gtk_box_pack_start(GTK_BOX(myVBox), myWindowToolbar.toolbarWidget(), false, false, 0);

	setPosition();
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
	GdkWindowState state = gdk_window_get_state(GTK_WIDGET(myMainWindow)->window);
	if (state & GDK_WINDOW_STATE_FULLSCREEN) {
		myWindowStateOption.setValue(FULLSCREEN);
	} else if (state & GDK_WINDOW_STATE_MAXIMIZED) {
		myWindowStateOption.setValue(MAXIMIZED);
	} else {
		myWindowStateOption.setValue(NORMAL);
		readPosition();
	}
}

void ZLGtkApplicationWindow::refresh() {
	ZLDesktopApplicationWindow::refresh();
	ToolbarType type =
		isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR;
	gtk_widget_queue_resize(toolbar(type).toolbarWidget());
}

void ZLGtkApplicationWindow::processAllEvents() {
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}

bool ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	GtkWidget *widget = gtk_window_get_focus(myMainWindow);
	if ((widget == 0) || !GTK_WIDGET_CAN_FOCUS(widget) || GTK_IS_DRAWING_AREA(widget)) {
		application().doActionByKey(ZLGtkKeyUtil::keyName(event));
		return true;
	}
	return false;
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

void ZLGtkApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	toolbar(type(button)).setToggleButtonState(button);
}

void ZLGtkApplicationWindow::onGtkButtonPress(GtkToolItem *gtkButton) {
	ToolbarType type =
		isFullscreen() ? FULLSCREEN_TOOLBAR : WINDOW_TOOLBAR;
	onButtonPress(toolbar(type).buttonItemByWidget(gtkButton));
	if (isFullscreen()) {
		gtk_window_present(myMainWindow);
	}
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == isFullscreen()) {
		return;
	}

	GdkWindowState state = gdk_window_get_state(GTK_WIDGET(myMainWindow)->window);
	if (fullscreen) {
		if ((state & GDK_WINDOW_STATE_MAXIMIZED) == 0) {
			readPosition();
		}
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(myWindowToolbar.toolbarWidget());
		if (myHandleBox != 0) {
			gtk_widget_show_all(GTK_WIDGET(myHandleBox));
		}
	} else {
		gtk_window_unfullscreen(myMainWindow);
		if (myHandleBox != 0) {
			gtk_widget_hide(GTK_WIDGET(myHandleBox));
		}
		gtk_widget_show(myWindowToolbar.toolbarWidget());
		if ((state & GDK_WINDOW_STATE_MAXIMIZED) == 0) {
			setPosition();
		}
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return
		gdk_window_get_state(GTK_WIDGET(myMainWindow)->window) &
		GDK_WINDOW_STATE_FULLSCREEN;
}

void ZLGtkApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	toolbar(type(*item)).addToolbarItem(item);
}

void ZLGtkApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	toolbar(type(*item)).setToolbarItemState(item, visible, enabled);
}

ZLViewWidget *ZLGtkApplicationWindow::createViewWidget() {
	myViewWidget = new ZLGtkViewWidget(&application(), (ZLView::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myVBox), myViewWidget->areaWithScrollbars());
	gtk_widget_show(myVBox);
	gtk_widget_show(myWindowToolbar.toolbarWidget());
	if (myHandleBox != 0) {
		gtk_widget_hide(GTK_WIDGET(myHandleBox));
	}
	return myViewWidget;
}

void ZLGtkApplicationWindow::close() {
	ZLGtkSignalUtil::removeAllSignals();
	gtk_main_quit();
}

void ZLGtkApplicationWindow::grabAllKeys(bool) {
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
		gdk_window_set_cursor(myViewWidget->area()->window, myHyperlinkCursor);
	} else {
		gdk_window_set_cursor(myViewWidget->area()->window, 0);
	}
}

void ZLGtkApplicationWindow::setFocusToMainWidget() {
	gtk_window_set_focus(myMainWindow, myViewWidget->area());
}

void ZLGtkApplicationWindow::readPosition() {
	int x, y, width, height;
	gtk_window_get_position(myMainWindow, &x, &y);
	gtk_window_get_size(myMainWindow, &width, &height);
	myXOption.setValue(x);
	myYOption.setValue(y);
	myWidthOption.setValue(width);
	myHeightOption.setValue(height);
}

void ZLGtkApplicationWindow::setPosition() {
	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_window_move(myMainWindow, myXOption.value(), myYOption.value());
}

