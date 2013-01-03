/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolBar>

#include "ZLQtDialogManager.h"
#include "ZLQtDialog.h"
#include "ZLQtOptionsDialog.h"
#include "ZLQtOpenFileDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtProgressDialog.h"
#include "ZLQtTreeDialog.h"

#include "../application/ZLQtApplicationWindow.h"
#include "../image/ZLQtImageManager.h"
#include "../util/ZLQtUtil.h"

void ZLQtDialogManager::showSearchBox() const {
	QLineEdit *searchBox = ZLQtApplicationWindow::Instance()->searchBox();
	searchBox->setFocus();
}

shared_ptr<ZLDialog> ZLQtDialogManager::createDialog(const ZLResourceKey &key) const {
	myStoredWindow = qApp->activeWindow();
	return new ZLQtDialog(resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLQtDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction) const {
	myStoredWindow = qApp->activeWindow();
	return new ZLQtOptionsDialog(ZLResource::resource(key), applyAction);
}

shared_ptr<ZLOpenFileDialog> ZLQtDialogManager::createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const {
	return new ZLQtOpenFileDialog(dialogTitle(key), directoryPath, filePath, filter);
}

shared_ptr<ZLTreeDialog> ZLQtDialogManager::createTreeDialog(const std::string &windowName, const ZLResource &resource) const {
	return new ZLQtTreeDialog(windowName, resource);
}

void ZLQtDialogManager::informationBox(const std::string &title, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	QMessageBox::information(parent, ::qtString(title), ::qtString(message), qtButtonText(OK_BUTTON));
}

void ZLQtDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	QMessageBox::critical(parent, ::qtString(dialogTitle(key)), ::qtString(message), qtButtonText(OK_BUTTON));
}

int ZLQtDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	QWidget *parent = qApp->activeWindow();
	if (parent == 0) {
		parent = myStoredWindow;
	}
	return QMessageBox::question(parent, ::qtString(dialogTitle(key)), ::qtString(message), qtButtonText(button0), qtButtonText(button1), qtButtonText(button2));
}

shared_ptr<ZLProgressDialog> ZLQtDialogManager::createProgressDialog(const ZLResourceKey &key, bool network) const {
	return new ZLQtProgressDialog(key, network);
}

bool ZLQtDialogManager::isClipboardSupported(ClipboardType type) const {
	return true;
}

void ZLQtDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if (!text.empty()) {
		qApp->clipboard()->setText(
			::qtString(text),
			(type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
		);
	}
}

void ZLQtDialogManager::setClipboardImage(const ZLImageData &imageData, ClipboardType type) const {
	qApp->clipboard()->setImage(
		*((ZLQtImageData&)imageData).image(),
		(type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
	);
}

QString ZLQtDialogManager::qtButtonText(const ZLResourceKey &key) {
	if (key.Name.empty()) {
		return QString::null;
	}
	return ::qtString(buttonText(key));
}
