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

#if MAEMO_VERSION == 2
	#include <hildon-widgets/gtk-infoprint.h>
	#include <hildon-note.h>
#elif MAEMO_VERSION == 4
	#include <hildon/hildon-note.h>
	#include <hildon/hildon-banner.h>
#else
	#error Unknown MAEMO_VERSION
#endif

#include "ZLGtkDialogManager.h"
#include "ZLGtkDialog.h"
#include "ZLGtkOptionsDialog.h"
#include "ZLGtkSelectionDialog.h"
#include "ZLGtkUtil.h"

shared_ptr<ZLDialog> ZLGtkDialogManager::createDialog(const ZLResourceKey &key) const {
	return new ZLGtkDialog(resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLGtkDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool) const {
	return new ZLGtkOptionsDialog(resource()[key], applyAction);
}

void ZLGtkDialogManager::informationBox(const ZLResourceKey&, const std::string &message) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_information(myWindow, message.c_str()));
	gtk_dialog_run(dialog);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ZLGtkDialogManager::errorBox(const ZLResourceKey&, const std::string &message) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_information_with_icon_name(myWindow, message.c_str(), GTK_STOCK_DIALOG_ERROR));
	gtk_dialog_run(dialog);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

int ZLGtkDialogManager::questionBox(const ZLResourceKey&, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	GtkDialog *dialog = GTK_DIALOG(hildon_note_new_confirmation_add_buttons(myWindow, message.c_str(), 0));

	if (!button0.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkButtonName(button0).c_str(), 0);
	}
	if (!button1.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkButtonName(button1).c_str(), 1);
	}
	if (!button2.Name.empty()) {
		gtk_dialog_add_button(dialog, gtkButtonName(button2).c_str(), 2);
	}

	gint response = gtk_dialog_run(dialog);

	gtk_widget_destroy(GTK_WIDGET(dialog));

	return response == GTK_RESPONSE_REJECT ? -1 : response;
}

bool ZLGtkDialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	return ZLGtkSelectionDialog(dialogTitle(key).c_str(), handler).run();
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

void ZLGtkDialogManager::wait(const ZLResourceKey &key, ZLRunnable &runnable) const {
	if (!myIsInitialized || myIsWaiting) {
		runnable.run();
	} else {
		myIsWaiting = true;
#if MAEMO_VERSION == 2
		gtk_banner_show_animation(myWindow, waitMessageText(key).c_str());
#elif MAEMO_VERSION == 4
		GtkWidget *banner = hildon_banner_show_animation(GTK_WIDGET(myWindow), NULL, waitMessageText(key).c_str());
#else
		#error Unknown value for MAEMO_VERSION
#endif
		RunnableWithFlag rwf;
		rwf.runnable = &runnable;
		rwf.flag = true;
		pthread_t thread;
		pthread_create(&thread, 0, runRunnable, &rwf);
		while (rwf.flag) {
			gtk_main_iteration();
		}
		pthread_join(thread, 0);
#if MAEMO_VERSION == 2
		gtk_banner_close(myWindow);
#elif MAEMO_VERSION == 4
		gtk_widget_destroy(banner);
#else
		#error Unknown value for MAEMO_VERSION
#endif
		myIsWaiting = false;
	}
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
