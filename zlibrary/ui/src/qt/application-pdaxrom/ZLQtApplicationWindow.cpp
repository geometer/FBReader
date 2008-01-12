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

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>

#include <ZLibrary.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../util/ZLQtKeyUtil.h"
#include "../view/ZLQtViewWidget.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQtApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Width", 10, 800, 350),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Height", 10, 800, 350),
	myFullScreen(false),
	myWasMaximized(false) {

	setWFlags(getWFlags() | WStyle_Customize);

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));

	resize(myWidthOption.value(), myHeightOption.value());

	qApp->setMainWidget(this);
	showMaximized();
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	if (!isFullscreen()) {
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
}

void ZLQtApplicationWindow::keyPressEvent(QKeyEvent *event) {
	application().doActionByKey(ZLQtKeyUtil::keyName(event));
}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
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

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLQtApplicationWindow::close() {
	QMainWindow::close();
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void ZLQtApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		static const std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
		const std::string &actionId = buttonItem.actionId();
		std::map<std::string,int>::const_iterator iter = myActionIndices.find(actionId);
		int actionIndex;
		if (iter != myActionIndices.end()) {
			actionIndex = iter->second;
		} else {
			actionIndex = myActionIndices.size() + 1;
			myActionIndices[actionId] = actionIndex;
			myActionIds[actionIndex] = actionId;
		}
		menuBar()->insertItem(QPixmap((imagePrefix + buttonItem.iconName() + ".png").c_str()), this, SLOT(emptySlot()), 0, actionIndex);
	}
}

void ZLQtApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			const std::string &actionId = button.actionId();
			const int id = myActionIndices[actionId];
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemVisible(id, application().isActionVisible(actionId));
				menuBar()->setItemEnabled(id, application().isActionEnabled(actionId));
			}
		}
	}
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	QMainWindow::setCaption(qCaption);
}

void ZLQtApplicationWindow::doActionSlot(int buttonNumber) {
	application().doAction(myActionIds[buttonNumber]);
}

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

ZLQtViewWidgetPositionInfo::ZLQtViewWidgetPositionInfo(const ZLQtApplicationWindow &window) : myWindow(window) {
}

int ZLQtViewWidgetPositionInfo::x() const {
	return 0;
}

int ZLQtViewWidgetPositionInfo::y() const {
	return ((myWindow.menuBar() != 0) && myWindow.menuBar()->isVisible()) ?
		myWindow.menuBar()->height() : 0;
}

int ZLQtViewWidgetPositionInfo::width() const {
	return myWindow.width();
}

int ZLQtViewWidgetPositionInfo::height() const {
	return myWindow.height() - y();
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidgetPositionInfo positionInfo(*this);
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application(), positionInfo);
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}
