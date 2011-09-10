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
#include <QtCore/QCoreApplication>

#include "ZLQmlProgressDialog.h"
#include "ZLQtUtil.h"

ZLQmlProgressDialog::ZLQmlProgressDialog(const ZLResourceKey &key) : ZLProgressDialog(key) {
	myText = QString::fromStdString(messageText());
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

ZLQmlProgressDialog::~ZLQmlProgressDialog() {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

QString ZLQmlProgressDialog::text() const {
	return myText;
}

void ZLQmlProgressDialog::run(ZLRunnable &runnable) {
	QEventLoop eventLoop;
	QThreadPool::globalInstance()->start(new ZLQmlRunnable(runnable, &eventLoop));
	eventLoop.exec(QEventLoop::AllEvents);
	emit finished();
	qApp->sendPostedEvents(0, QEvent::DeferredDelete);
}

void ZLQmlProgressDialog::finish() {
}

void ZLQmlProgressDialog::setMessage(const std::string &message) {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	const QString text = QString::fromStdString(message);
	if (text == myText)
		return;
	myText = text;
	emit textChanged(myText);
}

ZLQmlRunnable::ZLQmlRunnable(ZLRunnable &runnable, QObject *eventLoop)
    : myRunnable(runnable), myEventLoop(eventLoop) {
	setAutoDelete(true);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

ZLQmlRunnable::~ZLQmlRunnable() {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

void ZLQmlRunnable::run() {
	myRunnable.run();
	QMetaObject::invokeMethod(myEventLoop, "quit");
}
