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

#include "ZLGtkApplicationWindow.h"

#include "../util/ZLGtkKeyUtil.h"
#include "../util/ZLGtkSignalUtil.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../view/ZLGtkViewWidget.h"
#include "../../../../core/src/dialogs/ZLOptionView.h"

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

static bool handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	return ((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, gpointer data) {
	((ZLGtkApplicationWindow*)data)->handleScrollEventSlot(event);
}

static const std::string OPTIONS = "Options";

ZLGtkApplicationWindow::Toolbar::Toolbar(ZLGtkApplicationWindow *window) : myWindow(window), myWidgetCounter(0) {
	myGtkToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_toolbar_set_style(myGtkToolbar, GTK_TOOLBAR_ICONS);
}

GtkWidget *ZLGtkApplicationWindow::Toolbar::toolbarWidget() const {
	return GTK_WIDGET(myGtkToolbar);
}

ZLApplication::Toolbar::ButtonItem &ZLGtkApplicationWindow::Toolbar::buttonItemByWidget(GtkWidget *gtkButton) {
	return (ZLApplication::Toolbar::ButtonItem&)*myWidgetToButton[gtkButton];
}

void ZLGtkApplicationWindow::Toolbar::attachWidget(ZLOptionView&, GtkWidget *widget) {
	gtk_toolbar_append_widget(myGtkToolbar, widget, 0, 0);
	++myWidgetCounter;
}

void ZLGtkApplicationWindow::Toolbar::attachWidgets(ZLOptionView &view, GtkWidget *widget0, GtkWidget *widget1) {
	attachWidget(view, widget0);
	attachWidget(view, widget1);
}

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myHyperlinkCursor(0),
	myHyperlinkCursorIsUsed(false),
	myToolbar(this) {

	myMainWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	gtk_window_set_icon(myMainWindow, gdk_pixbuf_new_from_file(iconFileName.c_str(), 0));
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	gtk_box_pack_start(GTK_BOX(myVBox), myToolbar.toolbarWidget(), false, false, 0);

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
	GdkWindowState state = gdk_window_get_state(GTK_WIDGET(myMainWindow)->window);
	if (state & GDK_WINDOW_STATE_FULLSCREEN) {
		myWindowStateOption.setValue(FULLSCREEN);
	} else if (state & GDK_WINDOW_STATE_MAXIMIZED) {
		myWindowStateOption.setValue(MAXIMIZED);
	} else {
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

void ZLGtkApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	myToolbar.setToggleButtonState(button);
}

void ZLGtkApplicationWindow::Toolbar::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	GtkToggleButton *gtkButton = GTK_TOGGLE_BUTTON(myButtonToWidget[&(ZLApplication::Toolbar::Item&)button]);
	const bool isPressed = button.isPressed();
	if (gtk_toggle_button_get_active(gtkButton) != isPressed) {
		gtk_toggle_button_set_active(gtkButton, isPressed);
	}
}

void ZLGtkApplicationWindow::onGtkButtonPress(GtkWidget *gtkButton) {
	onButtonPress(myToolbar.buttonItemByWidget(gtkButton));
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == isFullscreen()) {
		return;
	}

	if (fullscreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(myToolbar.toolbarWidget());
	} else {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(myToolbar.toolbarWidget());
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return
		gdk_window_get_state(GTK_WIDGET(myMainWindow)->window) &
		GDK_WINDOW_STATE_FULLSCREEN;
}

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	myToolbar.addToolbarItem(item);
}

void ZLGtkApplicationWindow::Toolbar::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	switch (item->type()) {
		case ZLApplication::Toolbar::Item::OPTION_ENTRY:
			{
				shared_ptr<ZLOptionEntry> entry = ((const ZLApplication::Toolbar::OptionEntryItem&)*item).entry();
				ZLOptionView *view = createViewByEntry("", "", entry);
				if (view != 0) {
					myViews.push_back(view);
					entry->setVisible(true);
				}
			}
			break;
		case ZLApplication::Toolbar::Item::BUTTON:
			{
				const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
				static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
				GtkWidget *image = gtk_image_new_from_file((imagePrefix + buttonItem.iconName() + ".png").c_str());
				GtkWidget *button = buttonItem.isToggleButton() ? gtk_toggle_button_new() : gtk_button_new();
				gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
				GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
				gtk_container_add(GTK_CONTAINER(button), image);
				gtk_toolbar_append_widget(myGtkToolbar, button, buttonItem.tooltip().c_str(), 0);
				ZLGtkSignalUtil::connectSignal(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(onButtonClicked), myWindow);
				myButtonToWidget[&*item] = button;
				myWidgetToButton[button] = item;
				gtk_widget_show_all(button);
			}
			break;
		case ZLApplication::Toolbar::Item::SEPARATOR:
			mySeparatorMap[item] = myButtonToWidget.size() + myWidgetCounter;
			mySeparatorVisibilityMap.push_back(std::pair<ZLApplication::Toolbar::ItemPtr,bool>(item, false));
			break;
	}
}

void ZLGtkApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	myToolbar.setToolbarItemState(item, visible, enabled);
}

void ZLGtkApplicationWindow::Toolbar::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	switch (item->type()) {
		case ZLApplication::Toolbar::Item::OPTION_ENTRY:
			break;
		case ZLApplication::Toolbar::Item::BUTTON:
			{
				std::map<const ZLApplication::Toolbar::Item*,GtkWidget*>::const_iterator it = myButtonToWidget.find(&*item);
				if (it != myButtonToWidget.end()) {
					GtkWidget *gtkButton = it->second;
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
				}
			}
			break;
		case ZLApplication::Toolbar::Item::SEPARATOR:
			{
				std::map<const shared_ptr<ZLApplication::Toolbar::Item>,int>::iterator it = mySeparatorMap.find(item);
				if (it != mySeparatorMap.end()) {
					int index = it->second;
					std::vector<std::pair<ZLApplication::Toolbar::ItemPtr,bool> >::iterator jt;
					for (jt = mySeparatorVisibilityMap.begin(); jt != mySeparatorVisibilityMap.end(); ++jt) {
						if (jt->first == it->first) {
							break;
						}
						if (jt->second) {
							++index;
						}
					}
					if (visible) {
						if (!jt->second) {
							gtk_toolbar_insert_space(myGtkToolbar, index);
						}
					} else {
						if (jt->second) {
							gtk_toolbar_remove_space(myGtkToolbar, index);
						}
					}
					jt->second = visible;
				}
			}
			break;
	}
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
		gdk_window_set_cursor(GTK_WIDGET(myMainWindow)->window, myHyperlinkCursor);
	} else {
		gdk_window_set_cursor(GTK_WIDGET(myMainWindow)->window, 0);
	}
}
