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

#include <QtGui/QFileDialog>

#include "ZLQtOpenFileDialog.h"

ZLQtOpenFileDialog::ZLQtOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter) {
	myDialog = new QFileDialog();
}

ZLQtOpenFileDialog::~ZLQtOpenFileDialog() {
	delete myDialog;
}

bool ZLQtOpenFileDialog::runInternal() {
	return myDialog->exec();
}

std::string ZLQtOpenFileDialog::filePath() const {
	return std::string();
}

std::string ZLQtOpenFileDialog::directoryPath() const {
	return std::string();
}

void ZLQtOpenFileDialog::setPosition(int x, int y) {
}

void ZLQtOpenFileDialog::setSize(int width, int height) {
}

int ZLQtOpenFileDialog::x() const {
	return myDialog->x();
}

int ZLQtOpenFileDialog::y() const {
	return myDialog->y();
}

int ZLQtOpenFileDialog::width() const {
	return myDialog->width();
}

int ZLQtOpenFileDialog::height() const {
	return myDialog->height();
}
