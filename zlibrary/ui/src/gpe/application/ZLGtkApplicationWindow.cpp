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
#include <ZLUnicodeUtil.h>

#include "../view/ZLGtkViewWidget.h"
#include "../../gtk/util/ZLGtkKeyUtil.h"
#include "../../gtk/util/ZLGtkSignalUtil.h"

#include "ZLGtkApplicationWindow.h"

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLGtkApplicationWindow*)data)->application().closeView();
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLGtkApplicationWindow*)data)->application().refreshWindow();
}

static void actionSlot(GtkWidget*, GdkEventButton*, gpointer data) {
	((ZLApplication::Action*)data)->checkAndRun();
}

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(event);
}

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
	((ZLGtkViewWidget*)data)->onMousePressed(event);
}

static void mouseReleased(GtkWidget*, GdkEventButton *event, gpointer data) {
	((ZLGtkViewWidget*)data)->onMouseReleased(event);
}

static void mouseMoved(GtkWidget*, GdkEventMotion *event, gpointer data) {
	((ZLGtkViewWidget*)data)->onMouseMoved(event);
}


static const std::string OPTIONS = "Options";

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Height", 10, 2000, 800) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_box_pack_start(GTK_BOX(myVBox), GTK_WIDGET(myToolbar), false, false, 0);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);

	myFullScreen = false;

	gtk_window_set_icon_name(myMainWindow, (ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png").c_str());
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	if (!myFullScreen) {
		int width, height;
		gtk_window_get_size(myMainWindow, &width, &height);
		myWidthOption.setValue(width);
		myHeightOption.setValue(height);
	}
}

void ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(ZLGtkKeyUtil::keyName(event));
}

void ZLGtkApplicationWindow::close() {
	ZLGtkSignalUtil::removeAllSignals();
	gtk_main_quit();
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	if (myFullScreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + buttonItem.iconName() + ".png").c_str());
		GtkToolItem *button = gtk_tool_item_new();
		GtkWidget *ebox = gtk_event_box_new();

		gtk_container_add(GTK_CONTAINER(ebox), image);
		gtk_container_set_border_width(GTK_CONTAINER(button), 1);
		gtk_container_add(GTK_CONTAINER(button), ebox);

		gtk_tool_item_set_homogeneous(button, false);
		gtk_tool_item_set_expand(button, false);

		GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
		gtk_toolbar_insert(myToolbar, button, -1);
		shared_ptr<ZLApplication::Action> action = application().action(buttonItem.actionId());
		if (!action.isNull()) {
			ZLGtkSignalUtil::connectSignal(GTK_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), &*action);
		}
		myButtons[item] = button;
		gtk_widget_show_all(GTK_WIDGET(button));
	}
}

void ZLGtkApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
			GtkWidget *gtkButton = GTK_WIDGET(myButtons[*it]);
			if (gtkButton != 0) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				const std::string &id = button.actionId();
				if (application().isActionVisible(id)) {
					gtk_widget_show(gtkButton);
				} else {
					gtk_widget_hide(gtkButton);
				}
				/*
				 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
				 * does something strange if WIDGET is already insensitive.
				 */
				bool enabled = GTK_WIDGET_STATE(gtkButton) != GTK_STATE_INSENSITIVE;
				if (enabled != application().isActionEnabled(id)) {
					gtk_widget_set_sensitive(gtkButton, !enabled);
				}
			}
		}
	}
}

ZLViewWidget *ZLGtkApplicationWindow::createViewWidget() {
	ZLGtkViewWidget *viewWidget = new ZLGtkViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myVBox), viewWidget->area());
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), viewWidget);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "button_release_event", GTK_SIGNAL_FUNC(mouseReleased), viewWidget);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "motion_notify_event", GTK_SIGNAL_FUNC(mouseMoved), viewWidget);
	gtk_widget_show_all(myVBox);
	return viewWidget;
}

void ZLGtkApplicationWindow::grabAllKeys(bool) {
}
