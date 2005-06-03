/*
 * FBReader -- electronic book reader
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

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"
#include "GtkViewWidget.h"
#include "GtkPaintContext.h"

static ZLIntegerOption Width("Options", "Width", 800);
static ZLIntegerOption Height("Options", "Height", 800);

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

static void actionSlot(GtkWidget*, gpointer data) {
	ActionSlotData *uData = (ActionSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((GtkFBReader*)data)->handleKeySlot(key);
}

GtkFBReader::GtkFBReader() : FBReader(new GtkPaintContext()) {
	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	GtkWidget *vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), vbox);

	myMenu = gtk_menu_bar_new();

	buildMenu();
	gtk_box_pack_start(GTK_BOX(vbox), myMenu, false, false, 0);

	gtk_widget_show_all (myMenu);

	myToolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), myToolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(myToolbar), GTK_TOOLBAR_ICONS);
	createToolbar();

	myViewWidget = new GtkViewWidget(this);
	gtk_container_add(GTK_CONTAINER(vbox), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	gtk_window_resize(myMainWindow, Width.value(), Height.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);

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
	myKeyBindings["R"] = ACTION_ROTATE_SCREEN;
	myKeyBindings["Up"] = ACTION_SCROLL_BACKWARD;
	myKeyBindings["Down"] = ACTION_SCROLL_FORWARD;
	myKeyBindings["Escape"] = ACTION_CANCEL;
	myKeyBindings["<Shift>plus"] = ACTION_INCREASE_FONT;
	myKeyBindings["equal"] = ACTION_DECREASE_FONT;
	myKeyBindings["Return"] = ACTION_FULLSCREEN;

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

	gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(item), result);

	return result;
}

static void addMenuItem(GtkWidget *menu, const char *label, ActionSlotData *data) {
	GtkWidget *item = gtk_menu_item_new_with_label(label);
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), item);
	g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(actionSlot), data);
}

void GtkFBReader::buildMenu() {
	GtkWidget *submenu;

	submenu = makeSubmenu(myMenu, "Library");

	addMenuItem(submenu, "Open", getSlotData(ACTION_SHOW_COLLECTION));
	addMenuItem(submenu, "Add To...", getSlotData(ACTION_ADD_BOOK));

	submenu = makeSubmenu(myMenu, "Recent");

	// MSS: we do not use it now...

	addMenuItem(myMenu, "Preferences", getSlotData(ACTION_SHOW_OPTIONS));
	addMenuItem(myMenu, "Close", getSlotData(ACTION_CANCEL));
}

GtkFBReader::~GtkFBReader() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	Width.setValue(width);
	Height.setValue(height);

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
	if (QuitOnCancelOption.value() || (myMode != BOOK_TEXT_MODE)) {
		close();
	}
}

void GtkFBReader::fullscreenSlot() {
	myFullScreen = !myFullScreen;

	if (myFullScreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(myToolbar);
		gtk_widget_hide(myMenu);
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(myToolbar);
		gtk_widget_show(myMenu);
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
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
	GtkWidget *button = gtk_button_new();
	gtk_button_set_relief((GtkButton*)button, GTK_RELIEF_NONE);
	GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
	gtk_container_add(GTK_CONTAINER(button), image);
	gtk_container_add(GTK_CONTAINER(myToolbar), button);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(actionSlot), getSlotData(id));
	myButtons[id] = button;
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
	GtkDialog *findDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Text search", NULL, GTK_DIALOG_MODAL,
														"Go", GTK_RESPONSE_ACCEPT,
														NULL));

	gtk_signal_connect(GTK_OBJECT(findDialog), "key_press_event", G_CALLBACK(dialogDefaultKeys), NULL);

	GtkEntry *wordToSearch = GTK_ENTRY(gtk_entry_new());

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), GTK_WIDGET(wordToSearch), true, true, 0);
	gtk_entry_set_text (wordToSearch, SearchPatternOption.value().c_str());
	gtk_entry_set_activates_default(wordToSearch, TRUE);

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
		SearchPatternOption.setValue(gtk_entry_get_text(wordToSearch));		// TODO: stripWhiteSpace
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
