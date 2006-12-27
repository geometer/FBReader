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

#include <gtk/gtkbox.h>

#include "ZLGtkDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkDialog::ZLGtkDialog(const std::string &name) {
	myTab = new ZLGtkDialogContent();
	myDialog = createGtkDialog(name.c_str());
}

ZLGtkDialog::~ZLGtkDialog() {
	gtk_widget_destroy(GTK_WIDGET(myDialog));
}

void ZLGtkDialog::addButton(const std::string &text) {
	std::string buttonText = gtkString(text);
	gtk_dialog_add_button(myDialog, buttonText.c_str(), GTK_RESPONSE_ACCEPT);
}

bool ZLGtkDialog::run() {
	gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(((ZLGtkDialogContent*)myTab)->widget()), true, true, 0);
	gtk_widget_show_all(GTK_WIDGET(myDialog));
	return gtk_dialog_run(GTK_DIALOG(myDialog)) == GTK_RESPONSE_ACCEPT;
}
