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
#include <qaction.h>
#include <qlayout.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQtApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

ZLQtToolBarAction::ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLApplication::Toolbar::ButtonItem &item) : QAction(parent), myItem(item) {
	static std::string imagePrefix = ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + ZLApplication::ApplicationName() + ZLApplication::PathDelimiter;
	QPixmap icon((imagePrefix + myItem.iconName() + ".png").c_str());
	setIconSet(QIconSet(icon));
	QSize size = icon.size();
	QIconSet::setIconSize(QIconSet::Large, size);
	QIconSet::setIconSize(QIconSet::Small, size);
	setToggleAction(item.isToggleButton());
	connect(this, SIGNAL(activated()), this, SLOT(onActivated()));
}

void ZLQtToolBarAction::onActivated() {
	((ZLQtApplicationWindow*)parent())->onButtonPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	myActions[&button]->setOn(button.isPressed());
}

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 800),
	myFullScreen(false),
	myWasMaximized(false) {

	setWFlags(getWFlags() | WStyle_Customize);

	myToolBar = new QToolBar(this);
	myToolBar->boxLayout()->setMargin(5);
	myToolBar->boxLayout()->setSpacing(3);
	setToolBarsMovable(false);

	resize(myWidthOption.value(), myHeightOption.value());

	qApp->setMainWidget(this);
	menuBar()->hide();
	show();
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	if (!isFullscreen() && !isMaximized()) {
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
	for (std::map<const ZLApplication::Toolbar::Item*,ZLQtToolBarAction*>::iterator it = myActions.begin(); it != myActions.end(); ++it) {
		if (it->second != 0) {
			delete it->second;
		}
	}
}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
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

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLQtApplicationWindow::keyPressEvent(QKeyEvent *event) {
	application().doActionByKey(ZLQtKeyUtil::keyName(event));
}

void ZLQtApplicationWindow::wheelEvent(QWheelEvent *event) {
	if (event->orientation() == Vertical) {
		if (event->delta() > 0) {
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
		} else {
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
		}
	}
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void ZLQtApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->isButton()) {
		ZLApplication::Toolbar::ButtonItem &buttonItem = (ZLApplication::Toolbar::ButtonItem&)*item;
		ZLQtToolBarAction *action = new ZLQtToolBarAction(this, buttonItem);
		action->addTo(myToolBar);
		myActions[&*item] = action;
	} else {
		//myToolBar->addSeparator();
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	QAction *action = myActions[&*item];
	if (action != 0) {
		action->setEnabled(enabled);
		action->setVisible(visible);
	}
}

void ZLQtApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void ZLQtApplicationWindow::close() {
	QMainWindow::close();
}

bool ZLQtApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

bool ZLQtApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

bool ZLQtApplicationWindow::isMousePresented() const {
	return true;
}

bool ZLQtApplicationWindow::isKeyboardPresented() const {
	return true;
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QMainWindow::setCaption(QString::fromUtf8(caption.c_str()));
}
