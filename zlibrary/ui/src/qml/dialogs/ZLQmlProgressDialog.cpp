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

#include <QtCore/QThreadPool>
#include <QtCore/QEventLoop>

#include "ZLQmlProgressDialog.h"
#include "ZLQtUtil.h"

ZLQmlProgressDialog::ZLQmlProgressDialog(const ZLResourceKey &key) : ZLProgressDialog(key) {
	myText = QString::fromStdString(messageText());
}

ZLQmlProgressDialog::~ZLQmlProgressDialog() {
}

QString ZLQmlProgressDialog::text() const {
	return myText;
}

void ZLQmlProgressDialog::run(ZLRunnable &runnable) {
	QEventLoop eventLoop;
	QThreadPool::globalInstance()->start(new ZLQmlRunnable(runnable, &eventLoop));
	eventLoop.exec(QEventLoop::AllEvents);
	emit finished();
}

void ZLQmlProgressDialog::setMessage(const std::string &message) {
	const QString text = QString::fromStdString(message);
	if (text == myText)
		return;
	myText = text;
	emit textChanged(myText);
}

ZLQmlRunnable::ZLQmlRunnable(ZLRunnable &runnable, QObject *eventLoop)
    : myRunnable(runnable), myEventLoop(eventLoop) {
	setAutoDelete(true);
}

ZLQmlRunnable::~ZLQmlRunnable() {
}

void ZLQmlRunnable::run() {
	myRunnable.run();
	QMetaObject::invokeMethod(myEventLoop, "quit");
}
