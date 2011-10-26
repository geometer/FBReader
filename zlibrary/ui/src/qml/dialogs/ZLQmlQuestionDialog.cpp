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

#include "ZLQmlQuestionDialog.h"
#include <QtCore/QEventLoop>
#include <QtCore/QCoreApplication>

ZLQmlQuestionDialog::ZLQmlQuestionDialog(const QString &title, const QString &text,
                                         const QString &button0Text, const QString &button1Text, const QString &button2Text)
    : myTitle(title), myText(text), myResult(0)
{
	myButtons << button0Text << button1Text << button2Text;
	while (myButtons.last().isEmpty())
		myButtons.removeLast();
}

QString ZLQmlQuestionDialog::title() const {
	return myTitle;
}

QString ZLQmlQuestionDialog::text() const {
	return myText;
}

QStringList ZLQmlQuestionDialog::buttons() const {
	return myButtons;
}

void ZLQmlQuestionDialog::press(int index) {
	myResult = index;
	emit finished();
}

int ZLQmlQuestionDialog::run() {
    QEventLoop eventLoop;
	connect(this, SIGNAL(finished()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
    eventLoop.exec(QEventLoop::AllEvents);
	qApp->sendPostedEvents(0, QEvent::DeferredDelete);
	return myResult;
}
