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

#include <qpe/qpeapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qpe/resource.h>

#include <ZLibrary.h>
#include <ZLUnicodeUtil.h>

#include "ZLQtApplicationWindow.h"
#include "../view/ZLQtViewWidget.h"
#include "../../qt/util/ZLQtKeyUtil.h"

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *a) : ZLApplicationWindow(a), myCloseFlag(false) {
	myFullScreen = false;

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));

	((QPEApplication*)qApp)->showMainWidget(this);
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
}

void ZLQtApplicationWindow::keyPressEvent(QKeyEvent *event) {
	killTimers();
	application().doActionByKey(ZLQtKeyUtil::keyName(event));
}

void ZLQtApplicationWindow::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void ZLQtApplicationWindow::resizeEvent(QResizeEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
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

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLQtApplicationWindow::timerEvent(QTimerEvent *) {
	if (application().closeView()) {
		myCloseFlag = true;
	}
	killTimers();
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void ZLQtApplicationWindow::addToolbarItem(shared_ptr<ZLApplication::Toolbar::Item>) {
}

void ZLQtApplicationWindow::refresh() {
	const ZLApplication::Toolbar::ItemVector &items = application().toolbar().items();

	bool isVisibilityChanged = false;
	if (myToolbarMask.size() != items.size()) {
		isVisibilityChanged = true;
		myToolbarMask.clear();
		myToolbarMask.assign(items.size(), false);
	}
	std::vector<bool>::iterator bt = myToolbarMask.begin();
	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
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
			if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				if (*bt) {
					const QPixmap &pixmap = Resource::loadPixmap((ZLUnicodeUtil::toLower(ZLibrary::ApplicationName()) + ZLibrary::FileNameDelimiter + button.iconName()).c_str());
					const std::string &actionId = button.actionId();
					std::map<std::string,int>::const_iterator iter = myActionIndices.find(actionId);
					int actionIndex;
					if (iter != myActionIndices.end()) {
						actionIndex = iter->second;
					} else {
						actionIndex = myActionIndices.size() + 1;
						myActionIndices[actionId] = actionIndex;
						myActionIds[actionIndex] = actionId;
					}
					menuBar()->insertItem(pixmap, this, SLOT(emptySlot()), 0, actionIndex);
				}
				++bt;
			}
		}
		centralWidget()->show();
	}

	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->type() == ZLApplication::Toolbar::Item::BUTTON) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			const std::string &actionId = button.actionId();
			const int id = myActionIndices[actionId];
			if (menuBar()->findItem(id) != 0) {
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

void ZLQtApplicationWindow::grabAllKeys(bool grab) {
	if (grab) {
		QPEApplication::grabKeyboard();
	} else {
		QPEApplication::ungrabKeyboard();
	}
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void ZLQtApplicationWindow::close() {
	myCloseFlag = true;
	QMainWindow::close();
}
