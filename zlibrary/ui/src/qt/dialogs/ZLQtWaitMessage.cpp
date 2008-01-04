/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLQtWaitMessage.h"
#include "ZLQtUtil.h"

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
	QPoint position = main->mapToGlobal(main->pos());
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
	usleep(5000);
	qApp->processEvents();
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
