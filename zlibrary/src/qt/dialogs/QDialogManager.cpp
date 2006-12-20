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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "QDialogManager.h"
#include "QCommonDialog.h"
#include "QOptionsDialog.h"
#include "QOpenFileDialog.h"
#include "QWaitMessage.h"

ZLOptionsDialog *QDialogManager::createOptionsDialog(const std::string &id, const std::string &title) const {
	return new QOptionsDialog(id, title);
}

ZLDialog *QDialogManager::createDialog(const std::string &title) const {
	return new QCommonDialog(title);
}

int QDialogManager::infoBox(const InfoBoxType type, const std::string &title, const std::string &message, const std::string &button0, const std::string &button1, const std::string &button2) const {
	switch (type) {
		default:
		case INFORMATION_TYPE:
			return QMessageBox::information(qApp->mainWidget(), QString::fromUtf8(title.c_str()), QString::fromUtf8(message.c_str()), button0.c_str(), button1.c_str(), button2.c_str());
		case ERROR_TYPE:
			return QMessageBox::critical(qApp->mainWidget(), QString::fromUtf8(title.c_str()), QString::fromUtf8(message.c_str()), button0.c_str(), button1.c_str(), button2.c_str());
		case QUESTION_TYPE:
			return QMessageBox::question(qApp->mainWidget(), QString::fromUtf8(title.c_str()), QString::fromUtf8(message.c_str()), button0.c_str(), button1.c_str(), button2.c_str());
	}
}

void QDialogManager::openFileDialog(const std::string &title, const ZLTreeHandler &handler) const {
	QOpenFileDialog(title.c_str(), handler).runWithSize();
}

void QDialogManager::wait(ZLRunnable &runnable, const std::string &message) const {
	QWaitMessage waitMessage(message);
	runnable.run();
}
