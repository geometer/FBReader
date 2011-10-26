/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include "ZLQmlDialogManager.h"
#include "ZLQmlDialog.h"
#include "ZLQmlOptionsDialog.h"
#include "ZLQmlOpenFileDialog.h"
#include "ZLQmlDialogContent.h"
#include "ZLQmlProgressDialog.h"
#include "ZLQmlQuestionDialog.h"
#include "ZLQmlTree.h"
#include "ZLQtUtil.h"
#include <QtDeclarative/qdeclarative.h>

#include "../image/ZLQtImageManager.h"
#include "ZLQmlOptionView.h"

ZLQmlDialogManager::ZLQmlDialogManager() {
	connect(this, SIGNAL(privateInformationBoxRequested(QString,QString,QString)),
	        SIGNAL(informationBoxRequested(QString,QString,QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(privateErrorBoxRequested(QString,QString,QString)),
	        SIGNAL(errorBoxRequested(QString,QString,QString)), Qt::QueuedConnection);
	qmlRegisterUncreatableType<ZLQmlOptionView>("org.fbreader", 0, 14, "OptionView", "Uncreatable type");
	qmlRegisterUncreatableType<ZLQmlBoolean3OptionView>("org.fbreader", 0, 14, "BooleanOptionView", "Uncreatable type");
}

void ZLQmlDialogManager::kill(QObject *dialog) {
	qDebug() << "Try to kill" << dialog;
	dialog->deleteLater();
	connect(dialog, SIGNAL(destroyed(QObject*)), this, SLOT(onObjectDestroyed(QObject*)));
}

void ZLQmlDialogManager::onObjectDestroyed(QObject *object) {
	qDebug("Destroyed %p", object);
}

shared_ptr<ZLDialog> ZLQmlDialogManager::createDialog(const ZLResourceKey &key) const {
	ZLQmlDialog *dialog = new ZLQmlDialog(resource()[key]);
	new Event(dialog, this, &ZLQmlDialogManager::dialogRequested);
	return dialog;
}

shared_ptr<ZLOptionsDialog> ZLQmlDialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	ZLQmlOptionsDialog *dialog = new ZLQmlOptionsDialog(resource()[key], applyAction, showApplyButton);
	new Event(dialog, this, &ZLQmlDialogManager::optionsDialogRequested);
	return dialog;
}

shared_ptr<ZLOpenFileDialog> ZLQmlDialogManager::createOpenFileDialog(const ZLResourceKey &key, const std::string &directoryPath, const std::string &filePath, const ZLOpenFileDialog::Filter &filter) const {
	ZLQmlOpenFileDialog *dialog = new ZLQmlOpenFileDialog(dialogTitle(key), directoryPath, filePath, filter);
	new Event(dialog, this, &ZLQmlDialogManager::fileDialogRequested);
	return dialog;
}

shared_ptr<ZLTreeDialog> ZLQmlDialogManager::createTreeDialog() const {
	ZLQmlTreeDialog *dialog = new ZLQmlTreeDialog();
	new Event(dialog, this, &ZLQmlDialogManager::treeDialogRequested);
	return dialog;
}

shared_ptr<ZLDialogContent> ZLQmlDialogManager::createContent(const ZLResourceKey &key) const {
	return new ZLQmlDialogContent(resource()[key]);
}

void ZLQmlDialogManager::informationBox(const std::string &title, const std::string &message) const {
	emit const_cast<ZLQmlDialogManager*>(this)->privateInformationBoxRequested(QString::fromStdString(title),
	                                                                           QString::fromStdString(message),
	                                                                           ::qtButtonName(OK_BUTTON));
}

void ZLQmlDialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	emit const_cast<ZLQmlDialogManager*>(this)->privateErrorBoxRequested(QString::fromStdString(dialogTitle(key)),
	                                                                     QString::fromStdString(message),
	                                                                     ::qtButtonName(OK_BUTTON));
}

int ZLQmlDialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	ZLQmlQuestionDialog *dialog = new ZLQmlQuestionDialog(QString::fromStdString(dialogTitle(key)), QString::fromStdString(message),
	                                                      ::qtButtonName(button0), ::qtButtonName(button1), ::qtButtonName(button2));
	emit const_cast<ZLQmlDialogManager*>(this)->questionDialogRequested(dialog);
	int result = dialog->run();
	qDebug() << Q_FUNC_INFO << result;
	dialog->deleteLater();
	return result;
}

shared_ptr<ZLProgressDialog> ZLQmlDialogManager::createProgressDialog(const ZLResourceKey &key) const {
	ZLQmlProgressDialog *dialog = new ZLQmlProgressDialog(key);
	new Event(dialog, this, &ZLQmlDialogManager::progressDialogRequested);
	return dialog;
}

bool ZLQmlDialogManager::openURL(const std::string &url) const {
	return QDesktopServices::openUrl(QUrl::fromUserInput(QString::fromStdString(url)));
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

template <typename Method>
ZLQmlDialogManager::Event::Event(QObject *o, const ZLQmlDialogManager *p, Method m)
    : QEvent(eventType()), object(o), parent(const_cast<ZLQmlDialogManager*>(p)) {
	method = static_cast<DialogRequestedSignal>(m);
	qApp->postEvent(parent.data(), this);
}

ZLQmlDialogManager::Event::~Event() {
	if (parent && object)
		(parent.data()->*method)(object.data());
}

QEvent::Type ZLQmlDialogManager::Event::eventType() {
	static QEvent::Type type = static_cast<QEvent::Type>(registerEventType());
	return type;
}
