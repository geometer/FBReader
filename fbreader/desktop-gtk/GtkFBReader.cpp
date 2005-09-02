/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <gtk-desktop/GtkViewWidget.h>
#include <gtk-desktop/GtkPaintContext.h>

#include <abstract/ZLDeviceInfo.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static ZLIntegerOption Width("Options", "Width", 800);
static ZLIntegerOption Height("Options", "Height", 800);

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	((GtkFBReader*)data)->quitSlot();
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

static void handleKeyEvent(GtkWidget*, GdkEventKey *event, gpointer data) {
	((GtkFBReader*)data)->handleKeyEventSlot(event);
}

static void handleScrollEvent(GtkWidget*, GdkEventScroll *event, gpointer data) {
	((GtkFBReader*)data)->handleScrollEventSlot(event);
}

GtkFBReader::GtkFBReader(const std::string& bookToOpen) : FBReader(new GtkPaintContext(), bookToOpen) {
	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	GtkWidget *vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), vbox);

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

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKeyEvent), this);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "scroll_event", G_CALLBACK(handleScrollEvent), this);

	addKeyBinding("L", ACTION_SHOW_COLLECTION);
	addKeyBinding("Z", ACTION_SHOW_LAST_BOOKS);
	addKeyBinding("O", ACTION_SHOW_OPTIONS);
	addKeyBinding("Left", ACTION_UNDO);
	addKeyBinding("Right", ACTION_REDO);
	addKeyBinding("C", ACTION_SHOW_CONTENTS);
	addKeyBinding("F", ACTION_SEARCH);
	addKeyBinding("P", ACTION_FIND_PREVIOUS);
	addKeyBinding("N", ACTION_FIND_NEXT);
	addKeyBinding("D", ACTION_SHOW_HIDE_POSITION_INDICATOR);
	addKeyBinding("I", ACTION_SHOW_BOOK_INFO);
	addKeyBinding("A", ACTION_ADD_BOOK);
	addKeyBinding("R", ACTION_ROTATE_SCREEN);
	addKeyBinding("Page_Up", ACTION_LARGE_SCROLL_BACKWARD);
	addKeyBinding("Page_Down", ACTION_LARGE_SCROLL_FORWARD);
	addKeyBinding("Up", ACTION_SMALL_SCROLL_BACKWARD);
	addKeyBinding("Down", ACTION_SMALL_SCROLL_FORWARD);
	addKeyBinding("<Ctrl>Home", ACTION_SCROLL_TO_HOME);
	addKeyBinding("Home", ACTION_SCROLL_TO_START_OF_TEXT);
	addKeyBinding("End", ACTION_SCROLL_TO_END_OF_TEXT);
	addKeyBinding("Escape", ACTION_CANCEL);
	addKeyBinding("minus", ACTION_DECREASE_FONT);
	addKeyBinding("equal", ACTION_INCREASE_FONT);
	addKeyBinding("Return", ACTION_TOGGLE_FULLSCREEN);

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

GtkFBReader::~GtkFBReader() {
	if (!myFullScreen) {
		int width, height;
		gtk_window_get_size(myMainWindow, &width, &height);
		Width.setValue(width);
		Height.setValue(height);
	}

	for (std::map<ActionCode,ActionSlotData*>::iterator item = myActions.begin(); item != myActions.end(); ++item) {
		delete item->second;
	}

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

void GtkFBReader::handleKeyEventSlot(GdkEventKey *event) {
	//std::cerr << gtk_accelerator_name(event->keyval, (GdkModifierType)event->state) << "\n";
	std::map<std::pair<guint,GdkModifierType>,ActionCode>::const_iterator
		accelerator = myKeyBindings.find(std::pair<guint,GdkModifierType>(event->keyval, (GdkModifierType)event->state));

	if (accelerator != myKeyBindings.end()) {
		doAction(accelerator->second);
	}
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
	delete this;
	gtk_main_quit();
}

void GtkFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;

	if (myFullScreen) {
		gtk_window_fullscreen(myMainWindow);
		gtk_widget_hide(myToolbar);
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(myMainWindow);
		gtk_widget_show(myToolbar);
	}

	gtk_widget_queue_resize(GTK_WIDGET(myMainWindow));
}

bool GtkFBReader::isFullscreen() const {
	return myFullScreen;
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
	GtkDialog *findDialog = GTK_DIALOG(gtk_dialog_new());

	gtk_window_set_title(GTK_WINDOW(findDialog), "Text search");

	if (getMainWindow() != 0)
		gtk_window_set_transient_for(GTK_WINDOW(findDialog), getMainWindow());

	gtk_window_set_modal(GTK_WINDOW(findDialog), TRUE);

	if (ZLDeviceInfo::isKeyboardPresented()) {
		gtk_dialog_add_button (findDialog, GTK_STOCK_FIND, GTK_RESPONSE_ACCEPT);
	} else {
		gtk_dialog_add_button (findDialog, "Find", GTK_RESPONSE_ACCEPT);
	}

	gtk_signal_connect(GTK_OBJECT(findDialog), "key_press_event", G_CALLBACK(dialogDefaultKeys), NULL);

	GtkEntry *wordToSearch = GTK_ENTRY(gtk_entry_new());

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), GTK_WIDGET(wordToSearch), true, true, 0);
	gtk_entry_set_text (wordToSearch, SearchPatternOption.value().c_str());
	gtk_entry_set_activates_default(wordToSearch, TRUE);

	GtkWidget *ignoreCase, *wholeText, *backward, *thisSectionOnly = 0;

	bool showThisSectionOnlyOption = ((TextView*)myViewWidget->view())->hasMultiSectionModel();

	if (ZLDeviceInfo::isKeyboardPresented()) {
		ignoreCase = gtk_check_button_new_with_mnemonic("_Ignore case");
		wholeText = gtk_check_button_new_with_mnemonic("In w_hole text");
		backward = gtk_check_button_new_with_mnemonic("_Backward");
		if (showThisSectionOnlyOption) {
			thisSectionOnly = gtk_check_button_new_with_mnemonic("_This section only");
		}
	} else {
		ignoreCase = gtk_check_button_new_with_label("Ignore case");
		wholeText = gtk_check_button_new_with_label("In whole text");
		backward = gtk_check_button_new_with_label("Backward");
		if (showThisSectionOnlyOption) {
			thisSectionOnly = gtk_check_button_new_with_label("This section only");
		}
	}

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), ignoreCase, true, true, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(ignoreCase), SearchIgnoreCaseOption.value());

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), wholeText, true, true, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(wholeText), SearchInWholeTextOption.value());

	gtk_box_pack_start(GTK_BOX(findDialog->vbox), backward, true, true, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(backward), SearchBackwardOption.value());

	if (showThisSectionOnlyOption) {
		gtk_box_pack_start(GTK_BOX(findDialog->vbox), thisSectionOnly, true, true, 0);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(thisSectionOnly), SearchThisSectionOnlyOption.value());
	}

	gtk_widget_show_all(GTK_WIDGET(findDialog));

	if (gtk_dialog_run (GTK_DIALOG(findDialog)) == GTK_RESPONSE_ACCEPT) {
		SearchPatternOption.setValue(gtk_entry_get_text(wordToSearch));		// TODO: stripWhiteSpace
		SearchIgnoreCaseOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ignoreCase)));
		SearchInWholeTextOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wholeText)));
		SearchBackwardOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(backward)));
		if (showThisSectionOnlyOption) {
			SearchThisSectionOnlyOption.setValue(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(thisSectionOnly)));
		}
		((TextView*)myViewWidget->view())->search(
			SearchPatternOption.value(),
			SearchIgnoreCaseOption.value(),
			SearchInWholeTextOption.value(),
			SearchBackwardOption.value(),
			SearchThisSectionOnlyOption.value()
		);
	}

	gtk_widget_destroy (GTK_WIDGET(findDialog));
}

// vim:ts=2:sw=2:noet
