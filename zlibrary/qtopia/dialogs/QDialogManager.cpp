/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <qapplication.h>
#include <qmessagebox.h>

#include "QDialogManager.h"
#include "QOptionsDialog.h"
#include "QOpenFileDialog.h"

ZLOptionsDialog *QDialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new QOptionsDialog(id, title);
}

int QDialogManager::informationBox(const char *title, const char *message, const char *button0, const char *button1, const char *button2) const {
	return QMessageBox::information(qApp->mainWidget(), title, message, button0, button1, button2);
}

void QDialogManager::openFileDialog(const std::string &title, const ZLFileHandler &handler) const {
	QOpenFileDialog(title.c_str(), handler).run();
}
