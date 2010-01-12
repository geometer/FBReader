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

#include <unistd.h>

#include <qapplication.h>
#include <qcursor.h>
#include <qwidget.h>
#include <qpainter.h>

#include "ZLQtProgressDialog.h"
#include "ZLQtUtil.h"

ZLQtProgressDialog::ZLQtProgressDialog(const ZLResourceKey &key) : ZLProgressDialog(key), myWaitMessage(0) {
}

void ZLQtProgressDialog::run(ZLRunnable &runnable) {
	ZLQtWaitMessage waitMessage(messageText());
	myWaitMessage = &waitMessage;
	runnable.run();
	myWaitMessage = 0;
}

void ZLQtProgressDialog::setMessage(const std::string &message) {
	if (myWaitMessage == 0) {
		return;
	}

	myWaitMessage->myLabelText = ::qtString(message);
	myWaitMessage->repaint();
	QFontMetrics metrics = myWaitMessage->fontMetrics();
	const int w = metrics.width(myWaitMessage->myLabelText) + 20;
	const int h = metrics.height() + 20;
	myWaitMessage->resize(w, h);
	qApp->processEvents();
}

ZLQtWaitMessage::ZLQtWaitMessage(const std::string &message) : QWidget(0, 0, WStyle_Splash), myLabelText(::qtString(message)) {
	QWidget *main = qApp->mainWidget();
	if (main != 0) {
		myCursorIsStored = true;
		myStoredCursor = main->cursor();
		main->setCursor(Qt::waitCursor);
	} else {
		myCursorIsStored = false;
	}
	setCursor(Qt::waitCursor);

	qApp->processEvents();

	if (main == 0) {
		main = QApplication::desktop();
	}
	QPoint position = main->pos();
	QFontMetrics metrics = fontMetrics();
	const int w = metrics.width(myLabelText) + 20;
	const int h = metrics.height() + 20;
	resize(1, 1);
	show();
	setGeometry(
		position.x() + (main->width() - w) / 2,
		position.y() + (main->height() - h) / 2,
		w, h
	);

	qApp->processEvents();
	for (int i = 0; i < 6; ++i) {
		usleep(5000);
		qApp->processEvents();
	}
}

ZLQtWaitMessage::~ZLQtWaitMessage() {
	if (myCursorIsStored) {
		QWidget *main = qApp->mainWidget();
		if (main != 0) {
			main->setCursor(myStoredCursor);
		}
	}
}

void ZLQtWaitMessage::paintEvent(QPaintEvent *event) {
	QPainter painter;
	painter.begin(this);
	painter.drawText(10, fontMetrics().height() - fontMetrics().descent() + 10, myLabelText);
	painter.end();
}
