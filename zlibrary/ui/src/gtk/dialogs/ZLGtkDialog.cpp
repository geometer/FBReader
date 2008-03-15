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

#include <gtk/gtkbox.h>

#include <ZLDialogManager.h>

#include "ZLGtkDialog.h"
#include "ZLGtkDialogContent.h"
#include "ZLGtkUtil.h"

ZLGtkDialog::ZLGtkDialog(const ZLResource &resource) {
	myTab = new ZLGtkDialogContent(resource);
	myDialog = createGtkDialog(resource[ZLDialogManager::DIALOG_TITLE].value());
	myIsPacked = false;
}

ZLGtkDialog::~ZLGtkDialog() {
	destroyGtkDialog(myDialog);
}

void ZLGtkDialog::addButton(const ZLResourceKey &key, bool accept) {
	std::string buttonText = gtkString(ZLDialogManager::buttonName(key));
	gtk_dialog_add_button(myDialog, buttonText.c_str(), accept ? GTK_RESPONSE_ACCEPT : GTK_RESPONSE_REJECT);
}

bool ZLGtkDialog::run() {
	if (!myIsPacked) {
		gtk_box_pack_start(GTK_BOX(myDialog->vbox), GTK_WIDGET(((ZLGtkDialogContent*)myTab)->widget()), true, true, 0);
		myIsPacked = true;
	}
	gtk_widget_show_all(GTK_WIDGET(myDialog));
	return gtk_dialog_run(GTK_DIALOG(myDialog)) == GTK_RESPONSE_ACCEPT;
}
