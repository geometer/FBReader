/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QClipboard>
#include <QtGui/QDesktopWidget>

#include "ZLQmlDialogManager.h"
#include "ZLQmlDialog.h"
#include "ZLQmlOptionsDialog.h"
#include "ZLQmlOpenFileDialog.h"
#include "ZLQmlDialogContent.h"
#include "ZLQtProgressDialog.h"
#include "ZLQtUtil.h"
#include <QtDeclarative/qdeclarative.h>

#include "../image/ZLQtImageManager.h"
#include "ZLQmlOptionView.h"

ZLQmlDialogManager::ZLQmlDialogManager() {
	connect(this, SIGNAL(privateDialogRequested(QObject*)),
	        SIGNAL(dialogRequested(QObject*)), Qt::QueuedConnection);
	connect(this, SIGNAL(privateFileDialogRequested(QObject*)),
	        SIGNAL(fileDialogRequested(QObject*)), Qt::QueuedConnection);
	connect(this, SIGNAL(privateOptionsDialogRequested(QObject*)),
	        SIGNAL(optionsDialogRequested(QObject*)), Qt::QueuedConnection);
	qmlRegisterUncreatableType<ZLQmlOptionView>("org.fbreader", 0, 14, "OptionView", "Uncreatable type");
	qmlRegisterUncreatableType<ZLQmlBoolean3OptionView>("org.fbreader", 0, 14, "BooleanOptionView", "Uncreatable type");
}

shared_ptr<ZLDialog> ZLQmlDialogManager::createDialog(const ZLResourceKey &key) const {
	ZLQmlDialog *dialog = new ZLQmlDialog(resource()[key]);
	emit const_cast<ZLQmlDialogManager*>(this)->privateDialogRequested(dialog);
	return dialog;
}

shared_ptr<ZLOptionsDialog> ZLQmlDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	ZLQmlOptionsDialog *dialog = new ZLQmlOptionsDialog(resource()[key], applyAction, showApplyButton);
	emit const_cast<ZLQmlDialogManager*>(this)->privateOptionsDialogRequested(dialog);
	return dialog;
}

shared_ptr<ZLOpenFileDialog> ZLQmlDialogManager::createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const {
	ZLQmlOpenFileDialog *dialog = new ZLQmlOpenFileDialog(dialogTitle(key), directoryPath, filePath, filter);
	emit const_cast<ZLQmlDialogManager*>(this)->privateFileDialogRequested(dialog);
	return dialog;
}

void ZLQmlDialogManager::informationBox(const std::string &title, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	QMessageBox::information(parent, QString::fromStdString(title), QString::fromStdString(message), ::qtButtonName(OK_BUTTON));
}

void ZLQmlDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	QWidget *parent = qApp->activeWindow();
	QMessageBox::critical(parent, QString::fromStdString(dialogTitle(key)), QString::fromStdString(message), ::qtButtonName(OK_BUTTON));
}

int ZLQmlDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	QWidget *parent = qApp->activeWindow();
	return QMessageBox::question(parent, QString::fromStdString(dialogTitle(key)), QString::fromStdString(message), ::qtButtonName(button0), ::qtButtonName(button1), ::qtButtonName(button2));
}

shared_ptr<ZLProgressDialog> ZLQmlDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	return new ZLQtProgressDialog(key);
}

bool ZLQmlDialogManager::isClipboardSupported(ClipboardType type) const {
	return true;
}

void ZLQmlDialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if (text.empty())
		return;
	qApp->clipboard()->setText(
				QString::fromStdString(text),
				(type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
							  );
}

void ZLQmlDialogManager::setClipboardImage(const ZLImageData &imageData, ClipboardType type) const {
	qApp->clipboard()->setImage(
	            *static_cast<const ZLQtImageData&>(imageData).image(),
	            (type == CLIPBOARD_MAIN) ? QClipboard::Clipboard : QClipboard::Selection
	                           );
}
