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
#include <hildon-widgets/gtk-infoprint.h>

#include "GtkWaitMessage.h"

GtkWaitMessage::GtkWaitMessage(GtkWindow *parent, const std::string& message) : myParent(parent) {
	// gtk_banner_show_animation(myParent, message.c_str());
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}

	gtk_infoprint(myParent, message.c_str());

	while (gtk_events_pending()) {
		gtk_main_iteration();
		g_usleep(500);
	}
}

GtkWaitMessage::~GtkWaitMessage() {
	gtk_banner_close(myParent);
}
