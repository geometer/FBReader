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

#include <QtGui/QApplication>
#include <QtGui/QCursor>
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>

#include "ZLQtProgressDialog.h"
#include "ZLQtUtil.h"

ZLQtProgressDialog::ZLQtProgressDialog(const ZLResourceKey &key) : ZLProgressDialog(key), myWaitMessage(0) {
}

void ZLQtProgressDialog::run(ZLRunnable &runnable) {
	myWaitMessage = new ZLQtWaitMessage(messageText());
	runnable.run();
	delete myWaitMessage;
	myWaitMessage = 0;
}

void ZLQtProgressDialog::setMessage(const std::string &message) {
	if (myWaitMessage == 0) {
		return;
	}

	myWaitMessage->myLabel->setText(::qtString(message));

	myWaitMessage->myLayout->invalidate();
	myWaitMessage->repaint();
	qApp->processEvents();
}

ZLQtWaitMessage::ZLQtWaitMessage(const std::string &message) : QWidget(0, Qt::SplashScreen) {
	QWidget *main = qApp->activeWindow();
	if (main != 0) {
		myMainWidget = main;
		myStoredCursor = main->cursor();
		myMainWidget->setCursor(Qt::WaitCursor);
	} else {
		myMainWidget = 0;
	}
	setCursor(Qt::WaitCursor);

	qApp->processEvents();

	myLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	myLabel = new QLabel(::qtString(message), this);
	myLayout->addWidget(myLabel);

	if (main == 0) {
		main = QApplication::desktop();
	}
	move(
		main->x() + main->width() / 2 - myLabel->width() / 2 - 10,
		main->y() + main->height() / 2 - myLabel->height() / 2 - 10
	);
	show();

	qApp->processEvents();
	usleep(5000);
	qApp->processEvents();
}

ZLQtWaitMessage::~ZLQtWaitMessage() {
	if (myMainWidget != 0) {
		myMainWidget->setCursor(myStoredCursor);
	}
}
