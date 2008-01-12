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

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((ZLGtkApplicationWindow*)data)->application().refreshWindow();
}

static void actionSlot(GtkWidget*, gpointer data) {
	((ZLApplication::Action*)data)->checkAndRun();
}

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((ZLGtkApplicationWindow*)data)->handleKeyEventSlot(key);
}

static const std::string OPTIONS = "Options";

ZLGtkApplicationWindow::ZLGtkApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Width", 10, 800, 350),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Height", 10, 800, 350) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	myVBox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), myVBox);

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_box_pack_start(GTK_BOX(myVBox), GTK_WIDGET(myToolbar), false, false, 0);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_BOTH);

	myFullScreen = false;

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);
}

ZLGtkApplicationWindow::~ZLGtkApplicationWindow() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	myWidthOption.setValue(width);
	myHeightOption.setValue(height);
}

void ZLGtkApplicationWindow::handleKeyEventSlot(GdkEventKey *event) {
	application().doActionByKey(ZLGtkKeyUtil::keyName(event));
}

void ZLGtkApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		gtk_widget_hide(GTK_WIDGET(myToolbar));
		gtk_window_fullscreen(myMainWindow);
	} else {
		gtk_widget_show(GTK_WIDGET(myToolbar));
		gtk_window_unfullscreen(myMainWindow);
	}
}

bool ZLGtkApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLGtkApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		static const std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
		GtkWidget *image = gtk_image_new_from_file((imagePrefix + buttonItem.iconName() + ".png").c_str());
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
			ZLGtkSignalUtil::connectSignal(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(actionSlot), &*action);
		}
		myButtons[item] = button;
		gtk_widget_show_all(GTK_WIDGET(button));
	}
}

void ZLGtkApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
			GtkWidget *gtkButton = myButtons[*it];
			if (gtkButton != 0) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				const std::string &actionId = button.actionId();
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

void ZLGtkApplicationWindow::setCaption(const std::string &caption) {
	int utf8Length = ZLUnicodeUtil::utf8Length(caption);
	if (utf8Length <= 60) {
		gtk_window_set_title(myMainWindow, caption.c_str());
	} else {
		int len = ZLUnicodeUtil::length(caption, 57);
		std::string shortCaption = caption.substr(len) + "...";
		gtk_window_set_title(myMainWindow, shortCaption.c_str());
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
