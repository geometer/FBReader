/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtCore/QThreadPool>
#include <QtCore/QDebug>

#include "ZLQtProgressDialog.h"
#include "ZLQtUtil.h"

ZLQtProgressDialog::ZLQtProgressDialog(const ZLResourceKey &key, bool network) : ZLProgressDialog(key), myIsNetworkRunnable(network), myActiveWindow(0) {
}

void ZLQtProgressDialog::run(ZLRunnable &runnable) {
		myActiveWindow = qApp->activeWindow();
		qDebug() << Q_FUNC_INFO << myActiveWindow;
		if (myActiveWindow != 0) {
			myActiveWindow->setCursor(Qt::WaitCursor);
		}

		ZLQtWaitDialog* dialog = new ZLQtWaitDialog(messageText(), myActiveWindow);
		dialog->setCursor(Qt::WaitCursor);

		if (myIsNetworkRunnable) {
			dialog->show();
			runnable.run();
			dialog->hide();
		} else {
			ZLQtRunnableWrapper* wrapper = new ZLQtRunnableWrapper(runnable);
			wrapper->setAutoDelete(true);
			QObject::connect(wrapper, SIGNAL(finished()), dialog, SLOT(close()), Qt::QueuedConnection);
			QThreadPool::globalInstance()->start(wrapper);
			dialog->exec();
		}
		dialog->deleteLater();
		restoreCursor();
}

void ZLQtProgressDialog::restoreCursor() {
	if (myActiveWindow != 0) {
		myActiveWindow->setCursor(Qt::ArrowCursor);
	}
}


void ZLQtProgressDialog::setMessage(const std::string &message) {
	//qDebug() << QString::fromStdString(message);
	//TODO implement
}

ZLQtWaitDialog::ZLQtWaitDialog(const std::string &message, QWidget* parent) : QDialog(parent) {
		setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) ^ Qt::WindowCloseButtonHint); //hide close button
		myLayout = new QVBoxLayout;

		myLabel = new QLabel(::qtString(message));
		myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		myLabel->setWordWrap(true);

		myProgressBar = new QProgressBar;
		myProgressBar->setRange(0,0);
		//myProgressBar->setFixedWidth( qApp->desktop()->availableGeometry().width()*COEF_PROGRESS_BAR_WIDTH );

		myLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		myLayout->addWidget(myLabel);
		myLayout->addWidget(myProgressBar);

		this->setLayout(myLayout);
}

ZLQtRunnableWrapper::ZLQtRunnableWrapper(ZLRunnable &runnable) : myRunnable(runnable) {
}

void ZLQtRunnableWrapper::run() {
	myRunnable.run();
	emit finished();
}
