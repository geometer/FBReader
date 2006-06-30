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

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>

#include <qt/QViewWidget.h>
#include <qt/QKeyUtil.h>

#include "QApplicationWindow.h"
#include "../dialogs/QDialogManager.h"

void QDialogManager::createApplicationWindow(ZLApplication *application) const {
	new QApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

QApplicationWindow::QApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 800),
	myFullScreen(false),
	myWasMaximized(false) {

	setWFlags(getWFlags() | WStyle_Customize);

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));
	resize(myWidthOption.value(), myHeightOption.value());

	qApp->setMainWidget(this);
	show();
}

QApplicationWindow::~QApplicationWindow() {
	if (!isFullscreen() && !isMaximized()) {
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
}

void QApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		myWasMaximized = isMaximized();
		menuBar()->hide();
		showFullScreen();
	} else {
		menuBar()->show();
		showNormal();
		if (myWasMaximized) {
			showMaximized();
		}
	}
}

bool QApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void QApplicationWindow::keyPressEvent(QKeyEvent *event) {
	application().doActionByKey(QKeyUtil::keyName(event));
}

void QApplicationWindow::wheelEvent(QWheelEvent *event) {
	if (event->orientation() == Vertical) {
		if (event->delta() > 0) {
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
		} else {
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
		}
	}
}

void QApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void QApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		menuBar()->insertItem(QPixmap((ImageDirectory + "/" + application().name() + "/" + buttonItem.iconName() + ".png").c_str()), this, SLOT(emptySlot()), 0, buttonItem.actionId());
	}
}

void QApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemVisible(id, application().isActionVisible(id));
				menuBar()->setItemEnabled(id, application().isActionEnabled(id));
			}
		}
	}
}

void QApplicationWindow::doActionSlot(int buttonNumber) {
	application().doAction(buttonNumber);
}

ZLViewWidget *QApplicationWindow::createViewWidget() {
	QViewWidget *viewWidget = new QViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void QApplicationWindow::close() {
	QMainWindow::close();
}

bool QApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void QApplicationWindow::grabAllKeys(bool) {
}

bool QApplicationWindow::isFingerTapEventSupported() const {
	return false;
}
