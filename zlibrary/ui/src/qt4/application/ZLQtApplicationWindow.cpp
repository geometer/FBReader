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

#include <QtGui/QApplication>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtGui/QToolBar>
#include <QtGui/QLineEdit>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QLayout>
#include <QtGui/QWheelEvent>
#include <QtCore/QObjectList>

#include <ZLibrary.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQtApplicationWindow(application);
}

ZLQtToolBarAction::ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLToolbar::AbstractButtonItem &item) : QAction(parent), myItem(item) {
	static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	QPixmap icon((imagePrefix + myItem.iconName() + ".png").c_str());
	setIcon(QIcon(icon));
	QSize size = icon.size();
	if (item.type() == ZLToolbar::Item::TOGGLE_BUTTON) {
		setCheckable(true);
	}
	setToolTip(QString::fromUtf8(myItem.tooltip().c_str()));
	connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

void ZLQtToolBarAction::onActivated() {
	((ZLQtApplicationWindow*)parent())->onButtonPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	myActions[&button]->setChecked(button.isPressed());
}

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myFullScreen(false),
	myWasMaximized(false),
	myCursorIsHyperlink(false) {

	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	QPixmap icon(iconFileName.c_str());
	setWindowIcon(icon);

	//setWFlags(getWFlags() | WStyle_Customize);

	myToolBar = new QToolBar(this);
	myToolBar->setMovable(false);
	addToolBar(myToolBar);
	myToolBar->setIconSize(QSize(32, 32));

	resize(myWidthOption.value(), myHeightOption.value());
	move(myXOption.value(), myYOption.value());

	menuBar()->hide();
	show();
}

void ZLQtApplicationWindow::init() {
	ZLDesktopApplicationWindow::init();
	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			setFullscreen(true);
			break;
		case MAXIMIZED:
			showMaximized();
			break;
	}
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	if (isFullscreen()) {
		myWindowStateOption.setValue(FULLSCREEN);
	} else if (isMaximized()) {
		myWindowStateOption.setValue(MAXIMIZED);
	} else {
		myWindowStateOption.setValue(NORMAL);
		QPoint position = pos();
		if (position.x() != -1) {
			myXOption.setValue(position.x());
		}
		if (position.y() != -1) {
			myYOption.setValue(position.y());
		}
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
	for (std::map<const ZLToolbar::Item*,QAction*>::iterator it = myActions.begin(); it != myActions.end(); ++it) {
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
	if (event->orientation() == Qt::Vertical) {
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

void ZLQtApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	QAction *action = 0;

	switch (item->type()) {
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		{
			ZLToolbar::AbstractButtonItem &buttonItem = (ZLToolbar::AbstractButtonItem&)*item;
			action = new ZLQtToolBarAction(this, buttonItem);
			myToolBar->addAction(action);
			break;
		}
		case ZLToolbar::Item::TEXT_FIELD:
		{
			//ZLToolbar::TextFieldItem &textFieldItem =
			//	(ZLToolbar::TextFieldItem&)*item;
			//QLineEdit *edit = new QLineEdit(myToolBar);
			//edit->setMaxLength(textFieldItem.maxWidth());
			//action = myToolBar->addWidget(edit);
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			action = myToolBar->addSeparator();
			break;
	}

	if (action != 0) {
		myActions[&*item] = action;
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
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

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QMainWindow::setWindowTitle(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	if (hyperlink == myCursorIsHyperlink) {
		return;
	}
	myCursorIsHyperlink = hyperlink;
	if (hyperlink) {
		myStoredCursor = cursor();
		setCursor(Qt::PointingHandCursor);
	} else {
		setCursor(myStoredCursor);
	}
}
