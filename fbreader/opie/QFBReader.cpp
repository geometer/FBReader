/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <qpe/qpeapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qpe/resource.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdialog.h>

#include <abstract/ZLOptions.h>

#include <qtopia/QViewWidget.h>
#include <qtopia/QKeyUtil.h>

#include "QFBReader.h"

QApplicationWindow::QApplicationWindow(ZLApplication *a) : ZLApplicationWindow(a), myCloseFlag(false) {
	if (application().KeyboardControlOption.value()) {
		grabAllKeys(true);
	}

	myFullScreen = false;

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));
}

QApplicationWindow::~QApplicationWindow() {
	if (application().KeyboardControlOption.value()) {
		grabAllKeys(false);
	}
}

void QApplicationWindow::keyPressEvent(QKeyEvent *event) {
	killTimers();
	application().doActionByKey(QKeyUtil::keyName(event));
}

void QApplicationWindow::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void QApplicationWindow::resizeEvent(QResizeEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void QApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		menuBar()->hide();
		showNormal();
		showFullScreen();
	} else {
		menuBar()->show();
		showNormal();
		showMaximized();
	}
}

bool QApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void QApplicationWindow::timerEvent(QTimerEvent *) {
	if (application().closeView()) {
		myCloseFlag = true;
	}
	killTimers();
}

void QApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void QApplicationWindow::addToolbarItem(shared_ptr<ZLApplication::Toolbar::Item>) {
}

void QApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();

	bool isVisibilityChanged = false;
	if (myToolbarMask.size() != items.size()) {
		isVisibilityChanged = true;
		myToolbarMask.clear();
		myToolbarMask.assign(items.size(), false);
	}
	std::vector<bool>::iterator bt = myToolbarMask.begin();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			if (application().isActionVisible(button.actionId()) != *bt) {
				*bt = !*bt;
				isVisibilityChanged = true;
			}
			++bt;
		}
	}
	if (isVisibilityChanged) {
		bt = myToolbarMask.begin();
		centralWidget()->hide();
		menuBar()->clear();
		for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
			if ((*it)->isButton()) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				if (*bt) {
					const QPixmap &pixmap = Resource::loadPixmap((application().name() + "/" + button.iconName()).c_str());
					menuBar()->insertItem(pixmap, this, SLOT(emptySlot()), 0, button.actionId());
				}
				++bt;
			}
		}
		centralWidget()->show();
	}

	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemEnabled(id, application().isActionEnabled(id));
			}
		}
	}
}

void QApplicationWindow::setCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	QMainWindow::setCaption(qCaption);
}

void QApplicationWindow::doActionSlot(int buttonNumber) {
	application().doAction(buttonNumber);
}

bool QApplicationWindow::isFullKeyboardControlSupported() const {
	return true;
}

void QApplicationWindow::grabAllKeys(bool grab) {
	if (grab) {
		QPEApplication::grabKeyboard();
	} else {
		QPEApplication::ungrabKeyboard();
	}
}

ZLViewWidget *QApplicationWindow::createViewWidget() {
	QViewWidget *viewWidget = new QViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void QApplicationWindow::close() {
	myCloseFlag = true;
	QMainWindow::close();
}
