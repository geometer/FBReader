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
#include <gdk/gdkkeysyms.h>

#include <abstract/ZLDeviceInfo.h>

#include "GtkUtil.h"
#include "GtkDialogManager.h"

std::string gtkString(const std::string &str, bool useMnemonics) {
	int index = str.find('&');
	if (index == -1) {
		return str;
	}
	std::string result = str;
	result.erase(index, 1);
	if (useMnemonics) {
		result.insert(index, "_");
	}
	return result;
}

std::string gtkString(const std::string &str) {
	return gtkString(str, ZLDeviceInfo::isKeyboardPresented());
}

static bool dialogDefaultKeys(GtkWidget *dialog, GdkEventKey *key, gpointer) {
	if (!((GtkDialogManager&)GtkDialogManager::instance()).isKeyboardGrabbed() && (key->state == 0)) {
		if (key->keyval == GDK_Return) {
			gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
			return true;
		}

		if (key->keyval == GDK_Escape) {
			gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_REJECT);
			return true;
		}
	}

	return false;
}

GtkDialog *createGtkDialog(const std::string& title) {
	GtkWindow *dialog = GTK_WINDOW(gtk_dialog_new());
	gtk_window_set_title(dialog, title.c_str());
	
	GtkDialogManager &manager = (GtkDialogManager&)GtkDialogManager::instance();

	GtkWindow *window = !manager.myDialogs.empty() ? manager.myDialogs.top() : manager.myWindow;
	if (window != 0) {
		gtk_window_set_transient_for(dialog, window);
	}
	gtk_window_set_modal(dialog, TRUE);
	gtk_signal_connect(GTK_OBJECT(dialog), "key-press-event", G_CALLBACK(dialogDefaultKeys), NULL);

	((GtkDialogManager&)GtkDialogManager::instance()).myDialogs.push(dialog);
	return GTK_DIALOG(dialog);
}

void destroyGtkDialog(GtkDialog *dialog) {
	if (!((GtkDialogManager&)GtkDialogManager::instance()).myDialogs.empty()) {
		((GtkDialogManager&)GtkDialogManager::instance()).myDialogs.pop();
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
