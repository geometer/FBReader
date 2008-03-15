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

#include "ZLGtkDialogManager.h"
#include "ZLGtkDialog.h"
#include "ZLGtkOptionsDialog.h"
#include "ZLGtkSelectionDialog.h"
#include "ZLGtkWaitMessage.h"
#include "ZLGtkUtil.h"

shared_ptr<ZLDialog> ZLGtkDialogManager::createDialog(const ZLResourceKey &key) const {
	return new ZLGtkDialog(resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLGtkDialogManager::createOptionsDialog(const ZLResourceKey &id, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	return new ZLGtkOptionsDialog(resource()[id], applyAction, showApplyButton);
}

void ZLGtkDialogManager::informationBox(const ZLResourceKey &key, const std::string &message) const {
	internalBox(GTK_STOCK_DIALOG_INFO, key, message);
}

void ZLGtkDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	internalBox(GTK_STOCK_DIALOG_ERROR, key, message);
}

int ZLGtkDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	return internalBox(GTK_STOCK_DIALOG_QUESTION, key, message, button0, button1, button2);
}

int ZLGtkDialogManager::internalBox(const gchar *icon, const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	GtkDialog *dialog = createGtkDialog(dialogTitle(key));

	if (!button0.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkString(buttonName(button0)).c_str(), 0);
	}
	if (!button1.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkString(buttonName(button1)).c_str(), 1);
	}
	if (!button2.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkString(buttonName(button2)).c_str(), 2);
	}

	GtkWidget *contents = gtk_hbox_new(false, 10);
	gtk_container_set_border_width(GTK_CONTAINER(contents), 10);

	GtkWidget *image = gtk_image_new_from_stock(icon, GTK_ICON_SIZE_DIALOG);
	gtk_misc_set_alignment(GTK_MISC(image), 0.5, 0.0);

	GtkWidget *label = gtk_label_new(message.c_str());
	gtk_label_set_line_wrap(GTK_LABEL(label), true);
	gtk_box_pack_start(GTK_BOX(contents), image, false, false, 0);
	gtk_box_pack_start(GTK_BOX(contents), label, true, true, 0);
	gtk_box_pack_start(GTK_BOX(dialog->vbox), contents, true, true, 0);
	gtk_widget_show_all(GTK_WIDGET(dialog));
	gint response = gtk_dialog_run(dialog);
	destroyGtkDialog(dialog);

	return (response < 0) ? -1 : response;
}

bool ZLGtkDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	return ZLGtkSelectionDialog(dialogTitle(key), handler).runWithSize();
}

void ZLGtkDialogManager::wait(const ZLResourceKey &key, ZLRunnable &runnable) const {
	ZLGtkWaitMessage waitMessage(!myDialogs.empty() ? myDialogs.top() : myWindow, waitMessageText(key));
	runnable.run();
}

bool ZLGtkDialogManager::isClipboardSupported(ClipboardType) const {
	return true;
}

void ZLGtkDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if (!text.empty()) {
		GdkAtom atom = (type == CLIPBOARD_MAIN) ? GDK_SELECTION_CLIPBOARD : GDK_SELECTION_PRIMARY;
		GtkClipboard *clipboard = gtk_clipboard_get(atom);
		gtk_clipboard_set_text(clipboard, text.data(), text.length());
	}
}
