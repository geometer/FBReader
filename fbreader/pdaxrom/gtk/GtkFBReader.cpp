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

#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLDeviceInfo.h>

#include <gtk/GtkDialogManager.h>
#include <gtk/GtkKeyUtil.h>
#include <gtk-pdaxrom/GtkViewWidget.h>
#include <gtk-pdaxrom/GtkPaintContext.h>

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

static void actionSlot(GtkWidget*, gpointer data) {
	ActionSlotData *uData = (ActionSlotData*)data;
	uData->Reader->doAction(uData->Code);
}

static void handleKey(GtkWidget *, GdkEventKey *key, gpointer data) {
	((GtkFBReader*)data)->handleKeyEventSlot(key);
}

static const std::string OPTIONS = "Options";

GtkFBReader::GtkFBReader(const std::string& bookToOpen) :
	FBReader(new GtkPaintContext(), bookToOpen),
  myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 800, 350),
  myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 800, 350) {

	myMainWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);

	GtkWidget *vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(myMainWindow), vbox);

	myToolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), myToolbar, false, false, 0);
	gtk_toolbar_set_style(GTK_TOOLBAR(myToolbar), GTK_TOOLBAR_BOTH);

	myViewWidget = new GtkViewWidget(this, (ZLViewWidget::Angle)AngleStateOption.value());
	gtk_container_add(GTK_CONTAINER(vbox), ((GtkViewWidget*)myViewWidget)->area());
	gtk_signal_connect_after(GTK_OBJECT(((GtkViewWidget*)myViewWidget)->area()), "expose_event", GTK_SIGNAL_FUNC(repaint), this);

	myFullScreen = false;

	gtk_window_resize(myMainWindow, myWidthOption.value(), myHeightOption.value());
	initWindow(this);
	gtk_widget_show_all(GTK_WIDGET(myMainWindow));

	setMode(BOOK_TEXT_MODE);

	gtk_widget_add_events(GTK_WIDGET(myMainWindow), GDK_KEY_PRESS_MASK);

	gtk_signal_connect(GTK_OBJECT(myMainWindow), "key_press_event", G_CALLBACK(handleKey), this);
}

GtkFBReader::~GtkFBReader() {
	int width, height;
	gtk_window_get_size(myMainWindow, &width, &height);
	myWidthOption.setValue(width);
	myHeightOption.setValue(height);

	delete (GtkViewWidget*)myViewWidget;
}

void GtkFBReader::handleKeyEventSlot(GdkEventKey *event) {
	doActionByKey(GtkKeyUtil::keyName(event));
}

void GtkFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;
	if (myFullScreen) {
		gtk_widget_hide(myToolbar);
		gtk_window_fullscreen(myMainWindow);
	} else {
		gtk_widget_show(myToolbar);
		gtk_window_unfullscreen(myMainWindow);
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
	if (item->isButton()) {
		const Toolbar::ButtonItem &buttonItem = (const Toolbar::ButtonItem&)*item;
		GtkWidget *image = gtk_image_new_from_file((ImageDirectory + "/FBReader/" + buttonItem.iconName() + ".png").c_str());
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
		ActionCode id = (ActionCode)buttonItem.actionId();
		gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(actionSlot), new ActionSlotData(this, id));
		myButtons[item] = button;
	} else {
		// TODO: implement
	}
}

void GtkFBReader::refresh() {
	const Toolbar::ItemVector &items = toolbar().items();
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			GtkWidget *gtkButton = myButtons[*it];
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

void GtkFBReader::searchSlot() {
	GtkDialog *findDialog = ((GtkDialogManager&)GtkDialogManager::instance()).createDialog("Text search");

	if (ZLDeviceInfo::isKeyboardPresented()) {
		gtk_dialog_add_button (findDialog, GTK_STOCK_FIND, GTK_RESPONSE_ACCEPT);
	} else {
		gtk_dialog_add_button (findDialog, "Find", GTK_RESPONSE_ACCEPT);
	}

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
