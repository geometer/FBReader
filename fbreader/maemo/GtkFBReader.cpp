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

#include <abstract/ZLDeviceInfo.h>

#include <maemo/GtkViewWidget.h>
#include <maemo/GtkKeyUtil.h>
#include <maemo/GtkPaintContext.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static bool applicationQuit(GtkWidget*, GdkEvent *event, gpointer data) {
	return !((GtkFBReader*)data)->tryQuit();
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

static void handleKey(GtkWidget*, GdkEventKey *key, gpointer data) {
	((GtkFBReader*)data)->handleKeyEventSlot(key);
}

GtkFBReader::GtkFBReader(const std::string& bookToOpen) : FBReader(new GtkPaintContext(), bookToOpen) {
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

	addMenuItem(myMenu, "Book Info...",  getSlotData(ACTION_SHOW_BOOK_INFO));
	// MSS: this item can actually be disabled if we do not have table of contents
	addMenuItem(myMenu, "Table Of Contents", getSlotData(ACTION_SHOW_CONTENTS));

	submenu = makeSubmenu(myMenu, "Library");

	addMenuItem(submenu, "Open", getSlotData(ACTION_SHOW_COLLECTION));
	addMenuItem(submenu, "Recent", getSlotData(ACTION_SHOW_LAST_BOOKS));
	addMenuItem(submenu, "Add Book...", getSlotData(ACTION_ADD_BOOK));

	submenu = makeSubmenu(myMenu, "Find");

	addMenuItem(submenu, "Find Text...", getSlotData(ACTION_SEARCH));
	addMenuItem(submenu, "Find Next", getSlotData(ACTION_FIND_NEXT));
	addMenuItem(submenu, "Find Previous", getSlotData(ACTION_FIND_PREVIOUS));

	submenu = makeSubmenu(myMenu, "View");

	// MSS: these two actions can have a checkbox next to them
	addMenuItem(submenu, "Rotate Screen", getSlotData(ACTION_ROTATE_SCREEN));
	addMenuItem(submenu, "Full Screen", getSlotData(ACTION_TOGGLE_FULLSCREEN));
	// addMenuItem(submenu, "Toggle Indicator", getSlotData(ACTION_FULLSCREEN));

	// MSS: we do not use it now...
	// myRecentMenu = gtk_menu_item_new_with_label("Recent");
	// gtk_menu_shell_append(GTK_MENU_SHELL(myMenu), myRecentMenu);
	// gtk_widget_set_sensitive(myRecentMenu, FALSE);

	addMenuItem(myMenu, "Preferences...", getSlotData(ACTION_SHOW_OPTIONS));
	addMenuItem(myMenu, "Close", getSlotData(ACTION_QUIT));
}

GtkFBReader::~GtkFBReader() {
	for (std::map<ActionCode,ActionSlotData*>::iterator item = myActions.begin(); item != myActions.end(); ++item) {
		delete item->second;
	}

	delete (GtkViewWidget*)myViewWidget;
}

void GtkFBReader::handleKeyEventSlot(GdkEventKey *event) {
	doAction(GtkKeyUtil::keyName(event));
}

void GtkFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;

	hildon_appview_set_fullscreen(myAppView, myFullScreen);
	if (myFullScreen) {
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}
}

bool GtkFBReader::isFullscreen() const {
	return myFullScreen;
}

void GtkFBReader::quitSlot() {
	delete this;
	gtk_main_quit();
}

void GtkFBReader::addButton(ActionCode id, const std::string &name) {
	GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/FBReader/" + name + ".png").c_str());
	GtkToolItem *button = gtk_tool_item_new();
	GtkWidget *ebox = gtk_event_box_new();

	gtk_container_add(GTK_CONTAINER(ebox), image);
	gtk_container_add(GTK_CONTAINER(button), ebox);

	gtk_tool_item_set_homogeneous(button, false);
	gtk_tool_item_set_expand(button, false);


	GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
	gtk_toolbar_insert(myToolbar, button, -1);
	g_signal_connect(G_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), getSlotData(id));
	myButtons[id] = button;
}

void GtkFBReader::addButtonSeparator() {
	GtkToolItem *space = gtk_separator_tool_item_new();
	gtk_separator_tool_item_set_draw((GtkSeparatorToolItem*)space, false);
	gtk_toolbar_insert(myToolbar, space, -1);
}

void GtkFBReader::enableMenuButtons() {
	FBReader::enableMenuButtons();
	bool enableSpace = false;
	int itemNumber = gtk_toolbar_get_n_items(myToolbar);
	for (int i = 0; i < itemNumber; i++) {
		GtkToolItem *item = gtk_toolbar_get_nth_item(myToolbar, i);
		if (GTK_IS_SEPARATOR_TOOL_ITEM(item)) {
			gtk_tool_item_set_visible_horizontal(item, enableSpace);
			enableSpace = false;
		} else if (gtk_tool_item_get_visible_horizontal(item)) {
			enableSpace = true;
		}
	}
}

void GtkFBReader::setButtonVisible(ActionCode id, bool visible) {
	gtk_tool_item_set_visible_horizontal(myButtons[id], visible);
}

/*
 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
 * does something strange if WIDGET is already insensitive.
 */
void GtkFBReader::setButtonEnabled(ActionCode id, bool enable) {
	std::map<ActionCode,GtkToolItem*>::const_iterator it = myButtons.find(id);
	if (it != myButtons.end()) {
		bool enabled = GTK_WIDGET_STATE(it->second) != GTK_STATE_INSENSITIVE;
		if (enabled != enable) {
			gtk_widget_set_sensitive(GTK_WIDGET(it->second), enable);
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
		gtk_dialog_add_button (findDialog, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	} else {
		gtk_dialog_add_button (findDialog, "Find", GTK_RESPONSE_ACCEPT);
		gtk_dialog_add_button (findDialog, "Cancel", GTK_RESPONSE_REJECT);
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
