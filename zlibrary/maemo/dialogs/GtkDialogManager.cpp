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

#include <abstract/ZLOpenFileDialog.h>

#include "GtkDialogManager.h"
#include "GtkCommonDialog.h"
#include "GtkOptionsDialog.h"
#include "GtkOpenFileDialog.h"

ZLDialog *GtkDialogManager::createDialog(const std::string &title) const {
	return new GtkCommonDialog(title);
}

ZLOptionsDialog *GtkDialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new GtkOptionsDialog(id, title, myWindow);
}

int GtkDialogManager::questionBox(const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_confirmation_add_buttons(myWindow, message.c_str(), 0));

	gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

	if (!button0.empty()) {
		gtk_dialog_add_button(dialog, button0.c_str(), 0);
	}
	if (!button1.empty()) {
		gtk_dialog_add_button(dialog, button1.c_str(), 1);
	}
	if (!button2.empty()) {
		gtk_dialog_add_button(dialog, button2.c_str(), 2);
	}

	gint response = gtk_dialog_run(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));

	return response == GTK_RESPONSE_REJECT ? -1 : response;
}

void GtkDialogManager::openFileDialog(const std::string &title, const ZLTreeHandler &handler) const {
	GtkOpenFileDialog(title.c_str(), handler).runWithSize();
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

void GtkDialogManager::wait(ZLRunnable &runnable, const std::string &message) const {
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
