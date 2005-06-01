/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>

#include "GtkOpenFileDialog.h"

GtkOpenFileDialog::GtkOpenFileDialog(const char *caption, const ZLFileHandler &handler) : ZLOpenFileDialog(handler) {
  myDialog = gtk_file_chooser_dialog_new(caption,
                                       NULL,
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                       NULL);
}

GtkOpenFileDialog::~GtkOpenFileDialog(void) {
  gtk_widget_destroy(myDialog);
}

void GtkOpenFileDialog::run() {
	if (gtk_dialog_run(GTK_DIALOG(myDialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(myDialog));

		handler().accept(filename, false);

		g_free(filename);
	}
}

// vim:ts=2:sw=2:noet
