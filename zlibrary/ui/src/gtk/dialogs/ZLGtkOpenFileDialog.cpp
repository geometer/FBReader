/*
 * Copyright (C) 2010-2013 Geometer Plus <contact@geometerplus.com>
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
#include <ZLFile.h>

#include "ZLGtkOpenFileDialog.h"
#include "ZLGtkUtil.h"

static gboolean filterHandler(const GtkFileFilterInfo *info, void *data) {
	const ZLOpenFileDialog::Filter &filter =
		*(const ZLOpenFileDialog::Filter*)data;
	return filter.accepts(ZLFile(info->filename, ZLMimeType::get(info->mime_type)));
}

ZLGtkOpenFileDialog::ZLGtkOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter) {
	myDialog = GTK_DIALOG(gtk_file_chooser_dialog_new(
		title.c_str(), 0, GTK_FILE_CHOOSER_ACTION_OPEN,
		gtkString(ZLDialogManager::buttonName(ZLDialogManager::CANCEL_BUTTON)).c_str(),
			GTK_RESPONSE_CANCEL,
		gtkString(ZLDialogManager::buttonName(ZLDialogManager::OK_BUTTON)).c_str(),
			GTK_RESPONSE_ACCEPT,
		0
	));
	GtkFileChooser *chooser = GTK_FILE_CHOOSER(myDialog);
	gtk_file_chooser_set_current_folder(chooser, directoryPath.c_str());
	gtk_file_chooser_set_filename(chooser, filePath.c_str());
	GtkFileFilter *gtkFilter = gtk_file_filter_new();
	gtk_file_filter_add_custom(gtkFilter, (GtkFileFilterFlags)(GTK_FILE_FILTER_FILENAME | GTK_FILE_FILTER_MIME_TYPE), filterHandler, (void*)&filter, 0);
	gtk_file_chooser_set_filter(chooser, gtkFilter);
}

ZLGtkOpenFileDialog::~ZLGtkOpenFileDialog() {
	destroyGtkDialog(myDialog);
}

bool ZLGtkOpenFileDialog::runInternal() {
	const int code = gtk_dialog_run(myDialog);
	gtk_widget_hide(GTK_WIDGET(myDialog));
	return code == GTK_RESPONSE_ACCEPT;
}

std::string ZLGtkOpenFileDialog::filePath() const {
	return gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(myDialog));
}

std::string ZLGtkOpenFileDialog::directoryPath() const {
	return gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(myDialog));
}

void ZLGtkOpenFileDialog::setPosition(int x, int y) {
	//gtk_window_move(GTK_WINDOW(myDialog), x, y);
}

int ZLGtkOpenFileDialog::x() const {
	int _x;
	gtk_window_get_position(GTK_WINDOW(myDialog), &_x, 0);
	return _x;
}

int ZLGtkOpenFileDialog::y() const {
	int _y;
	gtk_window_get_position(GTK_WINDOW(myDialog), 0, &_y);
	return _y;
}

void ZLGtkOpenFileDialog::setSize(int width, int height) {
	gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int ZLGtkOpenFileDialog::width() const {
	int _width;
	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, 0);
	return _width;
}

int ZLGtkOpenFileDialog::height() const {
	int _height;
	gtk_window_get_size(GTK_WINDOW(myDialog), 0, &_height);
	return _height;
}
