/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <gtk/gtkmain.h>
#include <gtk/gtklabel.h>
#include <gdk/gdkcursor.h>

#include "ZLGtkProgressDialog.h"

ZLGtkProgressDialog::ZLGtkProgressDialog(GtkWindow *window, const ZLResourceKey &key) : ZLProgressDialog(key), myParent(window), myLabel(0) {
}

void ZLGtkProgressDialog::setMessage(const std::string &message) {
	if (myLabel == 0) {
		return;
	}

	gtk_label_set_text(GTK_LABEL(myLabel), message.c_str());
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}

void ZLGtkProgressDialog::run(ZLRunnable &runnable) {
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}

	GtkWindow *mainWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_POPUP));
	gtk_window_set_accept_focus(mainWindow, FALSE);
	myLabel = gtk_label_new(messageText().c_str());
	gtk_misc_set_padding(GTK_MISC(myLabel), 10, 10);    //  something nice?
	gtk_container_add(GTK_CONTAINER(mainWindow), myLabel);
	gtk_widget_show_all(GTK_WIDGET(mainWindow));

	GdkCursor *cursor = gdk_cursor_new(GDK_WATCH);

	if (myParent != 0) {
		gdk_window_set_cursor(GTK_WIDGET(myParent)->window, cursor);
	}

	gdk_window_set_cursor(GTK_WIDGET(mainWindow)->window, cursor);
	gdk_cursor_unref(cursor);

	int x, y, w, h;
	if (myParent != 0) {
		gtk_window_get_position(myParent, &x, &y);
		gtk_window_get_size(myParent, &w, &h);
		x += w / 2;
		y += h / 2;
	} else {
		GdkWindow *root = gdk_screen_get_root_window(gdk_screen_get_default());
		gdk_window_get_geometry(root, &x, &y, &w, &h, 0);
		x += w / 2;
		y += h / 2;
	}
	gtk_window_get_size(mainWindow, &w, &h);
	x -= w / 2;
	y -= h / 2;
	gtk_window_move(mainWindow, x, y);

	while (gtk_events_pending()) {
		gtk_main_iteration();
	}

	if (myParent != 0) {
		gtk_widget_queue_draw(GTK_WIDGET(myParent));
	}

	while (gtk_events_pending()) {
		gtk_main_iteration();
	}

	runnable.run();

	myLabel = 0;

	if (myParent != 0) {
		gdk_window_set_cursor(GTK_WIDGET(myParent)->window, 0);
	}
	gtk_widget_destroy(GTK_WIDGET(mainWindow));
}
