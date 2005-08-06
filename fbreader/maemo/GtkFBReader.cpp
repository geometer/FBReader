/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk-maemo/GtkViewWidget.h>
#include <gtk-maemo/GtkPaintContext.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->close();
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->repaintView();
}

struct ActionSlotData {
	ActionSlotData(GtkFBReader *reader, FBReader::ActionCode code) { Reader = reader; Code = code; }
	GtkFBReader *Reader;
	FBReader::ActionCode Code;
};

static void actionSlot(GtkWidget*, GdkEventButton*, gpointer data) {
	ActionSlotData *uData = (ActionSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

static void menuActionSlot(GtkWidget *, gpointer data) {
	ActionSlotData *uData = (ActionSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((GtkFBReader*)data)->handleKeySlot(key);
}

GtkFBReader::GtkFBReader() : FBReader(new GtkPaintContext()) {
	myApp = HILDON_APP(hildon_app_new());
	myAppView = HILDON_APPVIEW(hildon_appview_new(NULL));

	hildon_app_set_appview(myApp, myAppView);
	hildon_app_set_two_part_title(myApp, FALSE);

	myMenu = GTK_WIDGET(hildon_appview_get_menu(myAppView));

	buildMenu();

	gtk_widget_show_all(myMenu);

	myToolbar = (GtkToolbar*)gtk_toolbar_new();
	gtk_toolbar_set_show_arrow(myToolbar, false);
	gtk_toolbar_set_orientation(myToolbar, GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);
	createToolbar();

	hildon_appview_set_toolbar(myAppView, myToolbar);

	myViewWidget = new GtkViewWidget(this);
	gtk_container_add(GTK_CONTAINER(myAppView), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	gtk_widget_show_all(GTK_WIDGET(myApp));

	setMode(BOOK_TEXT_MODE);

	// MSS: for some reason it's not required for maemo platform
	// gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myApp), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
	gtk_signal_connect(GTK_OBJECT(myApp), "key_press_event", G_CALLBACK(handleKey), this);

	myKeyBindings["L"] = ACTION_SHOW_COLLECTION;
	myKeyBindings["O"] = ACTION_SHOW_OPTIONS;
	myKeyBindings["Left"] = ACTION_UNDO;
	myKeyBindings["Right"] = ACTION_REDO;
	myKeyBindings["C"] = ACTION_SHOW_CONTENTS;
	myKeyBindings["F"] = ACTION_SEARCH;
	myKeyBindings["P"] = ACTION_FIND_PREVIOUS;
	myKeyBindings["N"] = ACTION_FIND_NEXT;
	myKeyBindings["D"] = ACTION_SHOW_HIDE_POSITION_INDICATOR;
	myKeyBindings["I"] = ACTION_SHOW_BOOK_INFO;
	myKeyBindings["A"] = ACTION_ADD_BOOK;
	myKeyBindings["Return"] = ACTION_ROTATE_SCREEN;
	myKeyBindings["Up"] = ACTION_DECREASE_FONT;
	myKeyBindings["Down"] = ACTION_INCREASE_FONT;
	myKeyBindings["Escape"] = ACTION_CANCEL;
	myKeyBindings["F6"] = ACTION_FULLSCREEN;
	myKeyBindings["F7"] = ACTION_SCROLL_FORWARD;
	myKeyBindings["F8"] = ACTION_SCROLL_BACKWARD;

	myFullScreen = false;
}

ActionSlotData *GtkFBReader::getSlotData(ActionCode	id) {
	ActionSlotData *data = myActions[id];

	if (data == NULL) {
		data = new ActionSlotData(this, id);
		myActions[id] = data;
	}

	return data;
}

static GtkWidget *makeSubmenu(GtkWidget *menu, const char *label) {
	GtkWidget *result = gtk_menu_new();
	GtkWidget *item = gtk_menu_item_new_with_label(label);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), result);

	return result;
}

static void addMenuItem(GtkWidget *menu, const char *label, ActionSlotData *data) {
	GtkWidget *item = gtk_menu_item_new_with_label(label);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menuActionSlot), data);
}

void GtkFBReader::buildMenu() {
	GtkWidget *submenu;

	submenu = makeSubmenu(myMenu, "Library");

	addMenuItem(submenu, "Open", getSlotData(ACTION_SHOW_COLLECTION));
	addMenuItem(submenu, "Add To...", getSlotData(ACTION_ADD_BOOK));

	// MSS: we do not use it now...
	// myRecentMenu = gtk_menu_item_new_with_label("Recent");
	// gtk_menu_shell_append(GTK_MENU_SHELL(myMenu), myRecentMenu);
	// gtk_widget_set_sensitive(myRecentMenu, FALSE);

	addMenuItem(myMenu, "Preferences", getSlotData(ACTION_SHOW_OPTIONS));
	addMenuItem(myMenu, "Close", getSlotData(ACTION_CANCEL));
}

GtkFBReader::~GtkFBReader() {
	for (std::map<ActionCode,ActionSlotData*>::iterator item = myActions.begin(); item != myActions.end(); ++item) {
		delete item->second;
	}

	delete (GtkViewWidget*)myViewWidget;
}

void GtkFBReader::handleKeySlot(GdkEventKey *event) {
	std::map<std::string,ActionCode>::const_iterator accel;

	for (accel = myKeyBindings.begin(); accel != myKeyBindings.end() ; ++accel) {
		guint key;
		GdkModifierType mods;

		gtk_accelerator_parse(accel->first.c_str(), &key, &mods);

		if (event->keyval == key && (GdkModifierType)event->state == mods) {
			break;
		}
	}

	if (accel != myKeyBindings.end()) {
		doAction(accel->second);
	}
}

// MSS: fullscreen is implemented differently from Zaurus stuff:
//  -- fullscreen key toggles the full screen mode
//  -- cancel key always tries to quit the application
void GtkFBReader::cancelSlot() {
	if (myFullScreen) {
		myFullScreen = false;
		hildon_appview_set_fullscreen(myAppView, false);
		gtk_widget_show(GTK_WIDGET(myToolbar));
	} else if (QuitOnCancelOption.value() || (myMode != BOOK_TEXT_MODE)) {
		close();
	}
}

void GtkFBReader::fullscreenSlot() {
	myFullScreen = !myFullScreen;

	hildon_appview_set_fullscreen(myAppView, myFullScreen);
	if (myFullScreen) {
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}
}

void GtkFBReader::close() {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
	} else {
		delete this;
		gtk_main_quit();
	}
}

void GtkFBReader::addButton(ActionCode id, const std::string &name) {
	GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/FBReader/" + name + ".png").c_str());
	GtkToolItem *button = gtk_tool_item_new();
	GtkWidget *ebox = gtk_event_box_new();

	gtk_container_add(GTK_CONTAINER(ebox), image);
	gtk_container_add(GTK_CONTAINER(button), ebox);

	GdkImage *gdkImage = GTK_IMAGE(image)->data.image.image;
	int w = gdkImage->width;
	int h = gdkImage->height;
	gtk_widget_set_usize(ebox, w + 3, h);

	gtk_tool_item_set_homogeneous(button, FALSE);
	gtk_tool_item_set_expand(button, FALSE);


	GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
	gtk_toolbar_insert(myToolbar, button, -1);
	g_signal_connect(G_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), getSlotData(id));
	myButtons[id] = (GtkWidget*)button;
}

void GtkFBReader::addButtonSeparator() {
	/*
	GtkWidget *spaceWidget = gtk_vseparator_new();
	gtk_container_add(GTK_CONTAINER(space), spaceWidget);
	*/
	GtkToolItem *space = gtk_separator_tool_item_new();
	gtk_separator_tool_item_set_draw((GtkSeparatorToolItem*)space, false);
	gtk_toolbar_insert(myToolbar, space, -1);
}

void GtkFBReader::setButtonVisible(ActionCode id, bool visible) {
	if (visible) {
		gtk_widget_show(myButtons[id]);
	} else {
		gtk_widget_hide(myButtons[id]);
	}
}

/*
 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
 * does something strange if WIDGET is already insensitive.
 */
void GtkFBReader::setButtonEnabled(ActionCode id, bool enable) {
	std::map<ActionCode,GtkWidget*>::const_iterator it = myButtons.find(id);
	if (it != myButtons.end()) {
		bool enabled = GTK_WIDGET_STATE(it->second) != GTK_STATE_INSENSITIVE;
		if (enabled != enable) {
			gtk_widget_set_sensitive(it->second, enable);
		}
	}
}

static bool dialogDefaultKeys(GtkWidget *dialog, GdkEventKey *key, gpointer) {
	if (key->keyval == GDK_Return && key->state == 0) {
		gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

		return true;
	} else if (key->keyval == GDK_Escape && key->state == 0) {
		gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_REJECT);

		return true;
	}

	return false;
}

void GtkFBReader::searchSlot() {
	GtkDialog *findDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Text search", getMainWindow(), GTK_DIALOG_MODAL,
														"Find", GTK_RESPONSE_ACCEPT,
														"Cancel", GTK_RESPONSE_REJECT,
														NULL));

	gtk_signal_connect(GTK_OBJECT(findDialog), "key_press_event", G_CALLBACK(dialogDefaultKeys), NULL);

	GtkWidget *wordToSearch = gtk_entry_new();

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), wordToSearch, true, true, 0);
	gtk_entry_set_text (GTK_ENTRY(wordToSearch), SearchPatternOption.value().c_str());

	GtkWidget *ignoreCase = gtk_check_button_new_with_label ("Ignore case");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), ignoreCase, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ignoreCase), SearchIgnoreCaseOption.value());

	GtkWidget *wholeText = gtk_check_button_new_with_label ("In whole text");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), wholeText, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(wholeText), SearchInWholeTextOption.value());

	GtkWidget *backward = gtk_check_button_new_with_label ("Backward");
	gtk_box_pack_start(GTK_BOX(findDialog->vbox), backward, true, true, 0);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(backward), SearchBackwardOption.value());

	gtk_widget_show_all(GTK_WIDGET(findDialog));

	if (gtk_dialog_run (GTK_DIALOG(findDialog)) == GTK_RESPONSE_ACCEPT) {
		SearchPatternOption.setValue(gtk_entry_get_text(GTK_ENTRY(wordToSearch)));	// FIXME: stripWhiteSpace
		SearchIgnoreCaseOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ignoreCase)));
		SearchInWholeTextOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wholeText)));
		SearchBackwardOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(backward)));
		((TextView*)myViewWidget->view())->search(
			SearchPatternOption.value(),
			SearchIgnoreCaseOption.value(),
			SearchInWholeTextOption.value(),
			SearchBackwardOption.value()
		);
	}

	gtk_widget_destroy (GTK_WIDGET(findDialog));
}

// vim:ts=2:sw=2:noet
