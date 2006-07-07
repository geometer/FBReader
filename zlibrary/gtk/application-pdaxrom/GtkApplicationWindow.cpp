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

#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLDeviceInfo.h>

#include "../util/GtkKeyUtil.h"
#include "../util/GtkSignalUtil.h"
#include "../dialogs/GtkDialogManager.h"
#include "../view-pdaxrom/GtkViewWidget.h"

#include "GtkApplicationWindow.h"

void GtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = (new GtkApplicationWindow(application))->getMainWindow();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkApplicationWindow*)data)->application().closeView();
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkApplicationWindow*)data)->application().refreshWindow();
}

static void actionSlot(GtkWidget*, gpointer data) {
	((ZLApplication::Action*)data)->checkAndRun();
}

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((GtkApplicationWindow*)data)->handleKeyEventSlot(key);
}

static const std::string OPTIONS = "Options";

GtkApplicationWindow::GtkApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 800, 350),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 800, 350) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	myToolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(myVBox), myToolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(myToolbar), GTK_TOOLBAR_BOTH);

	myFullScreen = false;

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	GtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);
}

GtkApplicationWindow::~GtkApplicationWindow() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	myWidthOption.setValue(width);
	myHeightOption.setValue(height);
}

void GtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(GtkKeyUtil::keyName(event));
}

void GtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		gtk_widget_hide(myToolbar);
		gtk_window_fullscreen(myMainWindow);
	} else {
		gtk_widget_show(myToolbar);
		gtk_window_unfullscreen(myMainWindow);
	}
}

bool GtkApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void GtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/" + ZLApplication::ApplicationName() + "/" + buttonItem.iconName() + ".png").c_str());
		GtkWidget *button = gtk_button_new();
		gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
		GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
		gtk_container_add(GTK_CONTAINER(button), image);

		// toolbar with standard buttons is too wide for zaurus screen
		GdkImage *gdkImage = GTK_IMAGE(image)->data.image.image;
		int w = gdkImage->width;
		int h = gdkImage->height;
		gtk_widget_set_usize(button, w + 6, h + 6);

		gtk_container_add(GTK_CONTAINER(myToolbar), button);
		shared_ptr<ZLApplication::Action> action = application().action(buttonItem.actionId());
		if (!action.isNull()) {
			GtkSignalUtil::connectSignal(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(actionSlot), &*action);
		}
		myButtons[item] = button;
		gtk_widget_show_all(GTK_WIDGET(button));
	}
}

void GtkApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			GtkWidget *gtkButton = myButtons[*it];
			if (gtkButton != 0) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				int actionId = button.actionId();
				if (application().isActionVisible(actionId)) {
					gtk_widget_show(gtkButton);
				} else {
					gtk_widget_hide(gtkButton);
				}
				/*
				 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
				 * does something strange if WIDGET is already insensitive.
				 */
				bool enabled = GTK_WIDGET_STATE(gtkButton) != GTK_STATE_INSENSITIVE;
				if (enabled != application().isActionEnabled(actionId)) {
					gtk_widget_set_sensitive(gtkButton, !enabled);
				}
			}
		}
	}
}

void GtkApplicationWindow::setCaption(const std::string &caption) {
	int utf8Length = ZLUnicodeUtil::utf8Length(caption);
	if (utf8Length <= 60) {
		gtk_window_set_title(myMainWindow, caption.c_str());
	} else {
		int len = ZLUnicodeUtil::length(caption, 57);
		std::string shortCaption = caption.substr(len) + "...";
		gtk_window_set_title(myMainWindow, shortCaption.c_str());
	}
}

ZLViewWidget *GtkApplicationWindow::createViewWidget() {
	GtkViewWidget *viewWidget = new GtkViewWidget(&application(), (ZLViewWidget::Angle)application().AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myVBox), viewWidget->area());
	GtkSignalUtil::connectSignal(GTK_OBJECT(viewWidget->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);
	gtk_widget_show_all(myVBox);
	return viewWidget;
}

void GtkApplicationWindow::close() {
	GtkSignalUtil::removeAllSignals();
	gtk_main_quit();
}

bool GtkApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void GtkApplicationWindow::grabAllKeys(bool) {
}

bool GtkApplicationWindow::isFingerTapEventSupported() const {
	return false;
}
