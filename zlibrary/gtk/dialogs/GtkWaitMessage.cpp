/*
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

#include <gtk/gtkmain.h>
#include <gtk/gtklabel.h>

#include "GtkWaitMessage.h"

GtkWaitMessage::GtkWaitMessage(GtkWindow *parent, const std::string& message) {
	myWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_POPUP));
	GtkWidget *label = gtk_label_new(message.c_str());
	gtk_misc_set_padding(GTK_MISC(label), 10, 10);    //  something nice?
	gtk_container_add(GTK_CONTAINER(myWindow), label);
	gtk_widget_show_all(GTK_WIDGET(myWindow));

	int x, y, w, h;
	if (parent != 0) {
		gtk_window_get_position(parent, &x, &y);
		gtk_window_get_size(parent, &w, &h);
		x += w / 2;
		y += h / 2;
	} else {
		GdkWindow *root = gdk_screen_get_root_window(gdk_screen_get_default());
		gdk_window_get_geometry(root, &x, &y, &w, &h, 0);
		x += w / 2;
		y += h / 2;
	}
	gtk_window_get_size(myWindow, &w, &h);
	x -= w / 2;
	y -= h / 2;
	gtk_window_move(myWindow, x, y);

	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}

GtkWaitMessage::~GtkWaitMessage() {
	gtk_widget_destroy(GTK_WIDGET(myWindow));
}
