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

#include <ZLDialogManager.h>
#include <QtCore/QEventLoop>

#include "ZLQmlDialog.h"
#include "ZLQmlDialogContent.h"
#include "ZLQtUtil.h"

ZLQmlDialog::ZLQmlDialog(const ZLResource &resource) {
	myResult = false;
	myTitle = QString::fromStdString(resource[ZLDialogManager::DIALOG_TITLE].value());
	myTab = new ZLQmlDialogContent(resource, this);
}

ZLQmlDialog::~ZLQmlDialog() {
}

QString ZLQmlDialog::title() const {
	return myTitle;
}

QStringList ZLQmlDialog::buttonNames() const {
	return myButtonNames;
}

QStringList ZLQmlDialog::acceptButtons() const {
	return myAcceptButtons;
}

QObject *ZLQmlDialog::content() const {
	return static_cast<ZLQmlDialogContent*>(myTab);
}

void ZLQmlDialog::addButton(const ZLResourceKey &key, bool accept) {
	QString name = ::qtButtonName(key);
	myButtonNames << name;
	if (accept) {
		myAcceptButtons << name;
		emit acceptButtonsChanged(myAcceptButtons);
	}
	emit buttonNamesChanged(myButtonNames);
}

bool ZLQmlDialog::run() {
    QEventLoop eventLoop;
	connect(this, SIGNAL(finished()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
    eventLoop.exec(QEventLoop::AllEvents);
	return myResult;
}

void ZLQmlDialog::accept() {
	myResult = true;
	emit finished();
}

void ZLQmlDialog::reject() {
	myResult = false;
	emit finished();
}
