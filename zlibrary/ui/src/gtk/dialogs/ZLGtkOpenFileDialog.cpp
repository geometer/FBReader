/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include <gtk/gtkfilechooserdialog.h>

#include <ZLDialogManager.h>

#include "ZLGtkOpenFileDialog.h"
#include "ZLGtkUtil.h"

ZLGtkOpenFileDialog::ZLGtkOpenFileDialog(const std::string &title, const std::string &directoryPath) : myTitle(title), myDirectoryPath(directoryPath) {
}

bool ZLGtkOpenFileDialog::run() {
	GtkDialog *dialog = GTK_DIALOG(gtk_file_chooser_dialog_new(
		myTitle.c_str(), 0, GTK_FILE_CHOOSER_ACTION_OPEN,
		gtkString(ZLDialogManager::buttonName(ZLDialogManager::CANCEL_BUTTON)).c_str(),
			GTK_RESPONSE_CANCEL,
		gtkString(ZLDialogManager::buttonName(ZLDialogManager::OK_BUTTON)).c_str(),
			GTK_RESPONSE_ACCEPT,
		0
	));
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_current_folder(chooser, myDirectoryPath.c_str());
	const int code = gtk_dialog_run(dialog);
	myFilePath = gtk_file_chooser_get_filename(chooser);
	myDirectoryPath = gtk_file_chooser_get_current_folder(chooser);
	gtk_widget_hide(GTK_WIDGET(dialog));
	destroyGtkDialog(dialog);
	return code == GTK_RESPONSE_ACCEPT;
}

std::string ZLGtkOpenFileDialog::filePath() const {
	return myFilePath;
}

std::string ZLGtkOpenFileDialog::directoryPath() const {
	return myDirectoryPath;
}
