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
#include <hildon-widgets/gtk-infoprint.h>
#include <hildon-note.h>

#include "ZLGtkDialogManager.h"
#include "ZLGtkDialog.h"
#include "ZLGtkOptionsDialog.h"
#include "ZLGtkSelectionDialog.h"
#include "ZLGtkUtil.h"

shared_ptr<ZLDialog> ZLGtkDialogManager::createDialog(const std::string &title) const {
	return new ZLGtkDialog(title);
}

shared_ptr<ZLOptionsDialog> ZLGtkDialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new ZLGtkOptionsDialog(id, title, myWindow);
}

void ZLGtkDialogManager::informationBox(const std::string&, const std::string &message) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_information(myWindow, message.c_str()));
	gtk_dialog_run(dialog);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ZLGtkDialogManager::errorBox(const std::string&, const std::string &message) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_information_with_icon_name(myWindow, message.c_str(), GTK_STOCK_DIALOG_ERROR));
	gtk_dialog_run(dialog);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

int ZLGtkDialogManager::questionBox(const std::string&, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_confirmation_add_buttons(myWindow, message.c_str(), 0));

	if (!button0.empty()) {
		gtk_dialog_add_button(dialog, gtkString(button0).c_str(), 0);
	}
	if (!button1.empty()) {
		gtk_dialog_add_button(dialog, gtkString(button1).c_str(), 1);
	}
	if (!button2.empty()) {
		gtk_dialog_add_button(dialog, gtkString(button2).c_str(), 2);
	}

	gint response = gtk_dialog_run(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));

	return response == GTK_RESPONSE_REJECT ? -1 : response;
}

bool ZLGtkDialogManager::selectionDialog(const std::string &title, ZLTreeHandler &handler) const {
	return ZLGtkSelectionDialog(title.c_str(), handler).run();
}

struct RunnableWithFlag {
	ZLRunnable *runnable;
	bool flag;
};

static void *runRunnable(void *data) {
	RunnableWithFlag &rwf = *(RunnableWithFlag*)data;
	rwf.runnable->run();
	rwf.flag = false;
	pthread_exit(0);
	return 0;
}

void ZLGtkDialogManager::wait(ZLRunnable &runnable, const std::string &message) const {
	if (!myIsInitialized || myIsWaiting) {
		runnable.run();
	} else {
		myIsWaiting = true;
		gtk_banner_show_animation(myWindow, message.c_str());
		RunnableWithFlag rwf;
		rwf.runnable = &runnable;
		rwf.flag = true;
		pthread_t thread;
		pthread_create(&thread, 0, runRunnable, &rwf);
		while (rwf.flag) {
			gtk_main_iteration();
		}
		pthread_join(thread, 0);
		gtk_banner_close(myWindow);
		myIsWaiting = false;
	}
}
