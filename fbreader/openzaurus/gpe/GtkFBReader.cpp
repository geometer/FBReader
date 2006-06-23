/*
 * FBReader -- electronic book reader
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

#include <gtk/GtkKeyUtil.h>
#include <maemo/GtkViewWidget.h>
#include <maemo/GtkPaintContext.h>

#include "../../common/description/BookDescription.h"
#include "../../common/fbreader/BookTextView.h"
#include "../../common/fbreader/FootnoteView.h"
#include "../../common/fbreader/ContentsView.h"
#include "../../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static bool quitFlag = false;

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	if (!quitFlag) {
		((GtkFBReader*)data)->doAction(ACTION_QUIT);
	}
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->repaintView();
}

struct ActionSlotData {
	ActionSlotData(GtkFBReader *reader, ActionCode code) { Reader = reader; Code = code; }
	GtkFBReader *Reader;
	ActionCode Code;
};

static void actionSlot(GtkWidget *eventBox, GdkEventButton*, gpointer data) {
	ActionSlotData *uData = (ActionSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	((GtkFBReader*)data)->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, gpointer data) {
	((GtkFBReader*)data)->handleScrollEventSlot(event);
}

static const std::string OPTIONS = "Options";

GtkFBReader::GtkFBReader(const std::string& bookToOpen) :
	FBReader(new GtkPaintContext(), bookToOpen),
  myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
  myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 800) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	GtkWidget *vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), vbox);

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(myToolbar), false, false, 0);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	myViewWidget = new GtkViewWidget(this, (ZLViewWidget::Angle)AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(vbox), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	initWindow(this);
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "scroll_event", G_CALLBACK(handleScrollEvent), this);

	myFullScreen = false;

	gtk_window_set_icon_name(myMainWindow, (ImageDirectory + "/fbreader/FBReader.png").c_str());
}

ActionSlotData *GtkFBReader::getSlotData(ActionCode	id) {
	ActionSlotData *data = myActions[id];

	if (data == NULL) {
		data = new ActionSlotData(this, id);
		myActions[id] = data;
	}

	return data;
}

GtkFBReader::~GtkFBReader() {
	if (!myFullScreen) {
		int width, height;
		gtk_window_get_size(myMainWindow, &width, &height);
		myWidthOption.setValue(width);
		myHeightOption.setValue(height);
	}

	for (std::map<ActionCode,ActionSlotData*>::iterator item = myActions.begin(); item != myActions.end(); ++item) {
		delete item->second;
	}

	delete (GtkViewWidget*)myViewWidget;
}

void GtkFBReader::handleKeyEventSlot(GdkEventKey *event) {
	doActionByKey(GtkKeyUtil::keyName(event));
}

void GtkFBReader::handleScrollEventSlot(GdkEventScroll *event) {
	switch (event->direction) {
		case GDK_SCROLL_UP:
			doAction(ACTION_MOUSE_SCROLL_BACKWARD);
			break;
		case GDK_SCROLL_DOWN:
			doAction(ACTION_MOUSE_SCROLL_FORWARD);
			break;
		default:
			break;
	}
}

void GtkFBReader::quitSlot() {
	if (!quitFlag) {
		quitFlag = true;
		delete this;
		gtk_main_quit();
	}
}

void GtkFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;

	if (myFullScreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool GtkFBReader::isFullscreen() const {
	return myFullScreen;
}

void GtkFBReader::addToolbarItem(Toolbar::ItemPtr item) {
	if (item->isButton()) {
		const Toolbar::ButtonItem &buttonItem = (const Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/fbreader/" + buttonItem.iconName() + ".png").c_str());
		GtkToolItem *button = gtk_tool_item_new();
		GtkWidget *ebox = gtk_event_box_new();

		gtk_container_add(GTK_CONTAINER(ebox), image);
		gtk_container_set_border_width(GTK_CONTAINER(button), 1);
		gtk_container_add(GTK_CONTAINER(button), ebox);

		gtk_tool_item_set_homogeneous(button, false);
		gtk_tool_item_set_expand(button, false);

		GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
		gtk_toolbar_insert(myToolbar, button, -1);
		ActionCode id = (ActionCode)buttonItem.actionId();
		gtk_signal_connect(GTK_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), getSlotData(id));
		myButtons[item] = button;
	}
}

void GtkFBReader::refresh() {
	const Toolbar::ItemVector &items = toolbar().items();
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			GtkWidget *gtkButton = GTK_WIDGET(myButtons[*it]);
			if (gtkButton != 0) {
				const Toolbar::ButtonItem &button = (const Toolbar::ButtonItem&)**it;
				int id = button.actionId();
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
