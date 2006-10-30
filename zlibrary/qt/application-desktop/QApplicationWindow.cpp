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

#include <iostream>

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qaction.h>

#include "QApplicationWindow.h"
#include "../dialogs/QDialogManager.h"
#include "../view/QViewWidget.h"
#include "../util/QKeyUtil.h"

void QDialogManager::createApplicationWindow(ZLApplication *application) const {
	new QApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

ToolBarAction::ToolBarAction(QApplicationWindow *parent, const ZLApplication::Toolbar::ButtonItem &item) : QAction(parent), myItem(item) {
	connect(this, SIGNAL(activated()), this, SLOT(onActivated()));
}

void ToolBarAction::onActivated() {
	((QApplicationWindow*)parent())->application().doAction(myItem.actionId());
}

QApplicationWindow::QApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 800),
	myFullScreen(false),
	myWasMaximized(false) {

	setWFlags(getWFlags() | WStyle_Customize);

	myToolBar = new QToolBar(this);
	setToolBarsMovable(false);

	resize(myWidthOption.value(), myHeightOption.value());

	qApp->setMainWidget(this);
	menuBar()->hide();
	show();
}

QApplicationWindow::~QApplicationWindow() {
	if (!isFullscreen() && !isMaximized()) {
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
	for (std::map<ZLApplication::Toolbar::ItemPtr, class ToolBarAction*>::iterator it = myActions.begin(); it != myActions.end(); ++it) {
		if (it->second != 0) {
			delete it->second;
		}
	}
}

void QApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		myWasMaximized = isMaximized();
		myToolBar->hide();
		showFullScreen();
	} else {
		myToolBar->show();
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
		static std::string imagePrefix = ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + ZLApplication::ApplicationName() + ZLApplication::PathDelimiter;
		QPixmap icon((imagePrefix + buttonItem.iconName() + ".png").c_str());
		ToolBarAction *action = new ToolBarAction(this, buttonItem);
		action->setIconSet(QIconSet(icon));
		QSize size = icon.size();
		QIconSet::setIconSize(QIconSet::Large, size);
		QIconSet::setIconSize(QIconSet::Small, size);
		action->addTo(myToolBar);
		myActions[item] = action;
	}
}

void QApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			QAction *action = myActions[*it];
			if (action != 0) {
				int id = button.actionId();
				action->setEnabled(application().isActionEnabled(id));
				action->setVisible(application().isActionVisible(id));
			}
		}
	}
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

bool QApplicationWindow::isMousePresented() const {
	return true;
}

bool QApplicationWindow::isKeyboardPresented() const {
	return true;
}
