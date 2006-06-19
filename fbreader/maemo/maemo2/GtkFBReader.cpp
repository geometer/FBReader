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

#include <abstract/ZLDeviceInfo.h>

#include <maemo/GtkViewWidget.h>
#include <maemo/GtkKeyUtil.h>
#include <maemo/GtkPaintContext.h>
#include <maemo/GtkDialogManager.h>

#include "../../common/description/BookDescription.h"
#include "../../common/fbreader/BookTextView.h"
#include "../../common/fbreader/FootnoteView.h"
#include "../../common/fbreader/ContentsView.h"
#include "../../common/fbreader/CollectionView.h"
#include "GtkFBReader.h"

static bool quitFlag = false;

static bool acceptAction() {
	return
		!quitFlag &&
		GtkDialogManager::isInitialized() &&
		!((GtkDialogManager&)GtkDialogManager::instance()).isWaiting();
}

static bool applicationQuit(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((GtkFBReader*)data)->doAction(ACTION_QUIT);
	}
	return true;
}

static void repaint(GtkWidget*, GdkEvent*, gpointer data) {
	if (acceptAction()) {
		((GtkFBReader*)data)->repaintView();
	}
}

struct ActionSlotData {
	ActionSlotData(GtkFBReader *reader, ActionCode code) { Reader = reader; Code = code; }
	GtkFBReader *Reader;
	ActionCode Code;
};

static void actionSlot(GtkWidget*, GdkEventButton*, gpointer data) {
	if (acceptAction()) {
		ActionSlotData *uData = (ActionSlotData*)data;
		uData->Reader->doAction(uData->Code);
	}
}

static void menuActionSlot(GtkWidget *, gpointer data) {
	if (acceptAction()) {
		ActionSlotData *uData = (ActionSlotData*)data;
		uData->Reader->doAction(uData->Code);
	}
}

static void handleKey(GtkWidget*, GdkEventKey *key, gpointer data) {
	if (acceptAction()) {
		((GtkFBReader*)data)->handleKeyEventSlot(key);
	}
}

GtkFBReader::GtkFBReader(const std::string& bookToOpen) : FBReader(new GtkPaintContext(), bookToOpen) {
	myProgram = HILDON_PROGRAM(hildon_program_get_instance());
	g_set_application_name("");

	osso_initialize("FBReader", "0.0", false, 0);

	myWindow = HILDON_WINDOW(hildon_window_new());

	myMenu = GTK_MENU(gtk_menu_new());

	buildMenu();
	hildon_window_set_menu(myWindow, myMenu);

	initWindow(this);
	gtk_widget_show_all(GTK_WIDGET(myMenu));

	myToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_toolbar_set_show_arrow(myToolbar, false);
	gtk_toolbar_set_orientation(myToolbar, GTK_ORIENTATION_HORIZONTAL);
	gtk_toolbar_set_style(myToolbar, GTK_TOOLBAR_ICONS);

	hildon_window_add_toolbar(myWindow, myToolbar);

	myViewWidget = new GtkViewWidget(this, (ZLViewWidget::Angle)AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(myWindow), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	hildon_program_add_window(myProgram, myWindow);

	gtk_widget_show_all(GTK_WIDGET(myWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_signal_connect(GTK_OBJECT(myWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
	gtk_signal_connect(GTK_OBJECT(myWindow), "key_press_event", G_CALLBACK(handleKey), this);

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

GtkMenu *GtkFBReader::makeSubmenu(GtkMenu *menu, const char *label) {
	GtkMenu *submenu = GTK_MENU(gtk_menu_new());
	GtkMenuItem *item = GTK_MENU_ITEM(gtk_menu_item_new_with_label(label));

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(item));
	gtk_menu_item_set_submenu(item, GTK_WIDGET(submenu));

	return submenu;
}

void GtkFBReader::addMenuItem(GtkMenu *menu, const char *label, ActionCode code) {
	GtkMenuItem *item = GTK_MENU_ITEM(gtk_menu_item_new_with_label(label));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), GTK_WIDGET(item));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(menuActionSlot), getSlotData(code));
	myMenuItems[code] = item;
}

void GtkFBReader::buildMenu() {
	addMenuItem(myMenu, "Book Info...",  ACTION_SHOW_BOOK_INFO);
	// MSS: this item can actually be disabled if we do not have table of contents
	addMenuItem(myMenu, "Table Of Contents", ACTION_SHOW_CONTENTS);

	GtkMenu *librarySubmenu = makeSubmenu(myMenu, "Library");
	addMenuItem(librarySubmenu, "Open", ACTION_SHOW_COLLECTION);
	addMenuItem(librarySubmenu, "Open Previous", ACTION_OPEN_PREVIOUS_BOOK);
	addMenuItem(librarySubmenu, "Recent", ACTION_SHOW_LAST_BOOKS);
	addMenuItem(librarySubmenu, "Add Book...", ACTION_ADD_BOOK);

	GtkMenu *findSubmenu = makeSubmenu(myMenu, "Find");
	addMenuItem(findSubmenu, "Find Text...", ACTION_SEARCH);
	addMenuItem(findSubmenu, "Find Next", ACTION_FIND_NEXT);
	addMenuItem(findSubmenu, "Find Previous", ACTION_FIND_PREVIOUS);

	GtkMenu *viewSubmenu = makeSubmenu(myMenu, "View");
	// MSS: these two actions can have a checkbox next to them
	addMenuItem(viewSubmenu, "Rotate Screen", ACTION_ROTATE_SCREEN);
	addMenuItem(viewSubmenu, "Full Screen", ACTION_TOGGLE_FULLSCREEN);
	addMenuItem(viewSubmenu, "Toggle Indicator", ACTION_SHOW_HIDE_POSITION_INDICATOR);

	// MSS: we do not use it now...
	// myRecentMenu = gtk_menu_item_new_with_label("Recent");
	// gtk_menu_shell_append(GTK_MENU_SHELL(myMenu), myRecentMenu);
	// gtk_widget_set_sensitive(myRecentMenu, FALSE);

	addMenuItem(myMenu, "Preferences...", ACTION_SHOW_OPTIONS);
	addMenuItem(myMenu, "Close", ACTION_QUIT);
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

	if (myFullScreen) {
		gtk_window_fullscreen(GTK_WINDOW(myWindow));
		gtk_widget_hide(GTK_WIDGET(myToolbar));
	} else if (!myFullScreen) {
		gtk_window_unfullscreen(GTK_WINDOW(myWindow));
		gtk_widget_show(GTK_WIDGET(myToolbar));
	}
}

bool GtkFBReader::isFullscreen() const {
	return myFullScreen;
}

void GtkFBReader::quitSlot() {
	if (!quitFlag) {
		quitFlag = true;
		delete this;
		gtk_main_quit();
	}
}

void GtkFBReader::addToolbarItem(Toolbar::ItemPtr item) {
	GtkToolItem *gtkItem;
	if (item->isButton()) {
		const Toolbar::ButtonItem &buttonItem = (const Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/FBReader/" + buttonItem.iconName() + ".png").c_str());
		gtkItem = gtk_tool_item_new();
		GtkWidget *ebox = gtk_event_box_new();

		gtk_container_add(GTK_CONTAINER(ebox), image);
		gtk_container_add(GTK_CONTAINER(gtkItem), ebox);

		gtk_tool_item_set_homogeneous(gtkItem, false);
		gtk_tool_item_set_expand(gtkItem, false);

		GTK_WIDGET_UNSET_FLAGS(gtkItem, GTK_CAN_FOCUS);
		ActionCode id = (ActionCode)buttonItem.actionId();
		g_signal_connect(G_OBJECT(ebox), "button_press_event", GTK_SIGNAL_FUNC(actionSlot), getSlotData(id));
	} else {
		gtkItem = gtk_separator_tool_item_new();
		gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(gtkItem), false);
	}
	gtk_toolbar_insert(myToolbar, gtkItem, -1);
	myButtons[item] = gtkItem;
}

void GtkFBReader::refresh() {
	const Toolbar::ItemVector &items = toolbar().items();
	bool enableToolbarSpace = false;
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		GtkToolItem *toolItem = myButtons[*it];
		if ((*it)->isButton()) {
			const Toolbar::ButtonItem &button = (const Toolbar::ButtonItem&)**it;

			if (toolItem != 0) {
				gtk_tool_item_set_visible_horizontal(toolItem, button.isVisible());
				if (button.isVisible()) {
					enableToolbarSpace = true;
				}
				/*
				 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
				 * does something strange if WIDGET is already insensitive.
				 */
				bool enabled = GTK_WIDGET_STATE(toolItem) != GTK_STATE_INSENSITIVE;
				if (enabled != button.isEnabled()) {
					gtk_widget_set_sensitive(GTK_WIDGET(toolItem), !enabled);
				}
			}

			GtkMenuItem *item = myMenuItems[(ActionCode)button.actionId()];
			if (item != 0) {
				if (button.isVisible()) {
					gtk_widget_show(GTK_WIDGET(item));
				} else {
					gtk_widget_hide(GTK_WIDGET(item));
				}
				gtk_widget_set_sensitive(GTK_WIDGET(item), button.isEnabled());
			}
		} else {
			if (toolItem != 0) {
				gtk_tool_item_set_visible_horizontal(toolItem, enableToolbarSpace);
				enableToolbarSpace = false;
			}
		}
	}

	toolbar().reset();
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
		gtk_window_set_transient_for(GTK_WINDOW(findDialog), GTK_WINDOW(getMainWindow()));

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
