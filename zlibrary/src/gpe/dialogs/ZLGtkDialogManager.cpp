/*
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

#include <ZLSelectionDialog.h>

#include "ZLGtkDialogManager.h"
#include "ZLGtkDialog.h"
#include "ZLGtkOptionsDialog.h"
#include "ZLGtkSelectionDialog.h"
#include "ZLGtkWaitMessage.h"
#include "ZLGtkUtil.h"
#include "../application/ZLGtkApplicationWindow.h"

void ZLGtkDialogManager::createApplicationWindow(ZLApplication *application) const {
	myWindow = (new ZLGtkApplicationWindow(application))->getMainWindow();
}

shared_ptr<ZLDialog> ZLGtkDialogManager::createDialog(const std::string &title) const {
	return new ZLGtkDialog(title);
}

shared_ptr<ZLOptionsDialog> ZLGtkDialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new ZLGtkOptionsDialog(id, title);
}

void ZLGtkDialogManager::informationBox(const std::string &title, const std::string &message) const {
	internalBox(GTK_STOCK_DIALOG_INFO, title, message, gtkString("&Ok"));
}

void ZLGtkDialogManager::errorBox(const std::string &title, const std::string &message) const {
	internalBox(GTK_STOCK_DIALOG_ERROR, title, message, gtkString("&Ok"));
}

int ZLGtkDialogManager::questionBox(const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	return internalBox(GTK_STOCK_DIALOG_QUESTION, title, message, button0, button1, button2);
}

int ZLGtkDialogManager::internalBox(const gchar *icon, const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	GtkDialog *dialog = createGtkDialog(title.c_str());

	if (!button0.empty()) {
		gtk_dialog_add_button(dialog, button0.c_str(), 0);
	}
	if (!button1.empty()) {
		gtk_dialog_add_button(dialog, button1.c_str(), 1);
	}
	if (!button2.empty()) {
		gtk_dialog_add_button(dialog, button2.c_str(), 2);
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
	gtk_widget_destroy(GTK_WIDGET(dialog));

	return response == GTK_RESPONSE_REJECT ? -1 : response;
}

bool ZLGtkDialogManager::selectionDialog(const std::string &title, ZLTreeHandler &handler) const {
	return ZLGtkSelectionDialog(title.c_str(), handler).run();
}

void ZLGtkDialogManager::wait(ZLRunnable &runnable, const std::string &message) const {
	ZLGtkWaitMessage waitMessage(myWindow, message);
	runnable.run();
}
