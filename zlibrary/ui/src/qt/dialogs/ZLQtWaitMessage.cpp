/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <unistd.h>

#include <iostream>

#include <qapplication.h>
#include <qcursor.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>

#include "ZLQtWaitMessage.h"
#include "ZLQtUtil.h"

ZLQtWaitMessage::ZLQtWaitMessage(const std::string &message) : QWidget(0, 0, WStyle_Splash) {
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

	QHBoxLayout layout(this, 10);
	QLabel *label = new QLabel(::qtString(message), this);
	layout.add(label);

	if (main == 0) {
		std::cerr << "main is null\n";
	} else {
		std::cerr << "main is not null\n";
	}

	if (main == 0) {
		main = QApplication::desktop();
	}
	std::cerr << main->x() << ";" << main->y() << ";" << main->width() << ";" << main->height() << "\n";
	move(
		main->x() + main->width() / 2 - label->width() / 2 - 10,
		main->y() + main->height() / 2 - label->height() / 2 - 10
	);
	show();

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
