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

#include <abstract/ZLUnicodeUtil.h>

#include <gtk-pdaxrom/GtkViewWidget.h>
#include <gtk-pdaxrom/GtkPaintContext.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static ZLIntegerOption Width("Options", "Width", 350);
static ZLIntegerOption Height("Options", "Height", 350);

// MSS: probably we want to leave it as it is, as it's for Zaurus
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

	myToolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), myToolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(myToolbar), GTK_TOOLBAR_BOTH);
	createToolbar();

	myViewWidget = new GtkViewWidget(this);
	gtk_container_add(GTK_CONTAINER(vbox), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	myFullScreen = false;

	gtk_window_resize(myMainWindow, Width.value(), Height.value());
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);

	addKeyBinding("L", ACTION_SHOW_COLLECTION);
	addKeyBinding("Z", ACTION_SHOW_LAST_BOOKS);
	addKeyBinding("C", ACTION_SHOW_CONTENTS);
	addKeyBinding("F", ACTION_SEARCH);
	addKeyBinding("N", ACTION_FIND_NEXT);
	addKeyBinding("P", ACTION_FIND_PREVIOUS);
	addKeyBinding("O", ACTION_SHOW_OPTIONS);
	addKeyBinding("I", ACTION_SHOW_BOOK_INFO);
	addKeyBinding("D", ACTION_SHOW_HIDE_POSITION_INDICATOR);
	addKeyBinding("R", ACTION_ROTATE_SCREEN);
	addKeyBinding("A", ACTION_ADD_BOOK);
	addKeyBinding("1", ACTION_INCREASE_FONT);
	addKeyBinding("2", ACTION_DECREASE_FONT);
	addKeyBinding("Left", ACTION_UNDO);
	addKeyBinding("Right", ACTION_REDO);
	addKeyBinding("Up", ACTION_SCROLL_BACKWARD);
	addKeyBinding("Down", ACTION_SCROLL_FORWARD);
	addKeyBinding("Escape", ACTION_CANCEL);
	addKeyBinding("Return", ACTION_FULLSCREEN);
}

GtkFBReader::~GtkFBReader() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	Width.setValue(width);
	Height.setValue(height);

	delete (GtkViewWidget*)myViewWidget;
}

void GtkFBReader::addKeyBinding(guint keyval, GdkModifierType state, ActionCode code) {
	myKeyBindings[std::pair<guint,GdkModifierType>(keyval, state)] = code;
}

void GtkFBReader::addKeyBinding(const std::string &accelerator, ActionCode code) {
	guint keyval;
	GdkModifierType state;

	gtk_accelerator_parse(accelerator.c_str(), &keyval, &state);
	addKeyBinding(keyval, state, code);
}

void GtkFBReader::handleKeySlot(GdkEventKey *event) {
	std::map<std::pair<guint,GdkModifierType>,ActionCode>::const_iterator
		accelerator = myKeyBindings.find(std::pair<guint,GdkModifierType>(event->keyval, (GdkModifierType)event->state));

	if (accelerator != myKeyBindings.end()) {
		doAction(accelerator->second);
	}
}

void GtkFBReader::fullscreenSlot() {
	if (!myFullScreen) {
		myFullScreen = true;
		gtk_widget_hide(myToolbar);
		gtk_window_fullscreen(myMainWindow);
	}
}

void GtkFBReader::cancelSlot() {
	if (myFullScreen) {
		myFullScreen = false;
		gtk_widget_show(myToolbar);
		gtk_window_unfullscreen(myMainWindow);
	} else if (QuitOnCancelOption.value() || (myMode != BOOK_TEXT_MODE)) {
		close();
	}
}

void GtkFBReader::close() {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
	} else {
		quitSlot();
	}
}

void GtkFBReader::quitSlot() {
	delete this;
	gtk_main_quit();
}

void GtkFBReader::addButton(ActionCode id, const std::string &name) {
	GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/FBReader/" + name + ".png").c_str());
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
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(actionSlot), new ActionSlotData(this, id));
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
	GtkDialog *findDialog = GTK_DIALOG(gtk_dialog_new_with_buttons ("Text search", getMainWindow(), GTK_DIALOG_MODAL,
														"Go", GTK_RESPONSE_ACCEPT,
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

void GtkFBReader::setWindowCaption(const std::string &caption) {
	int utf8Length = ZLUnicodeUtil::utf8Length(caption);
	if (utf8Length <= 60) {
		gtk_window_set_title(myMainWindow, caption.c_str());
	} else {
		int l = ZLUnicodeUtil::length(caption, 57);
		std::string shortCaption = caption.substr(l) + "...";
		gtk_window_set_title(myMainWindow, shortCaption.c_str());
	}
}

// vim:ts=2:sw=2:noet
