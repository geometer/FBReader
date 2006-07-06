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
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>

#include "QApplicationWindow.h"
#include "QtMenuAction.h"
#include "../view/QViewWidget.h"
#include "../dialogs/QDialogManager.h"
#include "../../qt/util/QKeyUtil.h"

class MyMenuBar : public QPEMenuBar {

public:
	MyMenuBar(QWidget *parent) : QPEMenuBar(parent) {}

private:
	void keyPressEvent(QKeyEvent *event) {
		if (event->key() == Key_Escape) {
			QPEMenuBar::keyPressEvent(event);
		}
	}
};

QApplicationWindow::QApplicationWindow(ZLApplication *a) : ZLApplicationWindow(a) {
	setWFlags(getWFlags() | WStyle_Customize);

	myFullScreen = false;
	myTitleHeight = -1;

	myVerticalDelta = -1;
	myHorizontalDelta = -1;

	myToolBar = new MyMenuBar(this);
	myMenu = new QPopupMenu(myToolBar);

	connect(myToolBar, SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));

	((QPEApplication*)qApp)->showMainWidget(this);
}

QApplicationWindow::~QApplicationWindow() {
}

void QApplicationWindow::setCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	QMainWindow::setCaption(qCaption);
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

void QApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr) {
}

void QApplicationWindow::initMenu() {
}

QApplicationWindow::MenuMaskCalculator::MenuMaskCalculator(QApplicationWindow &window) : myWindow(window), myFirstTime(myWindow.myMenuMask.empty()), myShouldBeUpdated(myFirstTime), myCounter(0) {
}

void QApplicationWindow::MenuMaskCalculator::processSubmenuBeforeItems(ZLApplication::Menubar::Submenu&) {
}

void QApplicationWindow::MenuMaskCalculator::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
}

void QApplicationWindow::MenuMaskCalculator::processSepartor(ZLApplication::Menubar::Separator&) {
}

void QApplicationWindow::MenuMaskCalculator::processItem(ZLApplication::Menubar::PlainItem &item) {
	bool visible = myWindow.application().isActionVisible(item.actionId());
	if (myFirstTime) {
		myWindow.myMenuMask.push_back(visible);
	} else {
		if (myWindow.myMenuMask[myCounter] != visible) {
			myWindow.myMenuMask[myCounter] = visible;
			myShouldBeUpdated = true;
		}
		++myCounter;
	}
}

QApplicationWindow::MenuUpdater::MenuUpdater(QApplicationWindow &window) : myWindow(window), myCounter(0) {
	myMenuStack.push(myWindow.myMenu);
}

void QApplicationWindow::MenuUpdater::processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu) {
	QPopupMenu *qmenu = new QPopupMenu(myMenuStack.top());
	myMenuStack.top()->insertItem(submenu.menuName().c_str(), qmenu);
	myMenuStack.push(qmenu);
}

void QApplicationWindow::MenuUpdater::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
	myMenuStack.pop();
}

void QApplicationWindow::MenuUpdater::processItem(ZLApplication::Menubar::PlainItem &item) {
	if (myWindow.myMenuMask[myCounter++]) {
		const int id = item.actionId();
		QtMenuAction *action = myWindow.myMenuMap[id];
		if (action == 0) {
			action = new QtMenuAction(myWindow.application(), item);
			myWindow.myMenuMap[id] = action;
		}
		action->addTo(myMenuStack.top());
		action->setEnabled(myWindow.application().isActionEnabled(id));
	}
}

void QApplicationWindow::MenuUpdater::processSepartor(ZLApplication::Menubar::Separator&) {
	myMenuStack.top()->insertSeparator();
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
		if (centralWidget() != 0) {
			centralWidget()->hide();
		}
		myToolBar->clear();
		for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
			if ((*it)->isButton()) {
				const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
				if (*bt) {
					const QPixmap &pixmap = Resource::loadPixmap((application().name() + '/' + button.iconName()).c_str());
					myToolBar->insertItem(pixmap, this, SLOT(emptySlot()), 0, button.actionId());
				}
				++bt;
			}
		}
		myToolBar->insertItem(QString::null, myMenu, -1, 0);
		if (centralWidget() != 0) {
			centralWidget()->show();
		}
	}

	for (ZLApplication::Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const ZLApplication::Toolbar::ButtonItem &button = (const ZLApplication::Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (myToolBar->findItem(id) != 0) {
				myToolBar->setItemEnabled(id, application().isActionEnabled(id));
			}
		}
	}

	MenuMaskCalculator calculator(*this);
	calculator.processMenu(application().menubar());
	if (calculator.shouldBeUpdated()) {
		myMenu->clear();
		MenuUpdater(*this).processMenu(application().menubar());
	} else {
		for (std::map<int,QtMenuAction*>::iterator it = myMenuMap.begin(); it != myMenuMap.end(); ++it) {
			it->second->setEnabled(application().isActionEnabled(it->first));
		}
	}
}

void QApplicationWindow::doActionSlot(int buttonNumber) {
	application().doAction(buttonNumber);
}

void QApplicationWindow::keyPressEvent(QKeyEvent *event) {
	application().doActionByKey(QKeyUtil::keyName(event));
}

void QApplicationWindow::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

int QApplicationWindow::veritcalAdjustment() {
	if (myFullScreen || (myVerticalDelta == -1)) {
		return 0;
	}
	return qApp->desktop()->height() - myVerticalDelta - height();
}

void QApplicationWindow::resizeEvent(QResizeEvent *event) {
	if ((myVerticalDelta == -1) && !myFullScreen) {
		myVerticalDelta = qApp->desktop()->height() - event->size().height();
		myHorizontalDelta = qApp->desktop()->width() - event->size().width();
	}
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		int titleHeight = topData()->normalGeometry.top();
		if (titleHeight > 0) {
			myTitleHeight = titleHeight;
		}
		topData()->normalGeometry = QRect(0, 0, -1, -1);
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
		myToolBar->hide();
		showFullScreen();
	} else {
		myToolBar->show();
		showNormal();
		if (myTitleHeight > 0) {
			move(1, myTitleHeight);
			myTitleHeight = -1;
		}
		setWFlags(getWFlags() | WStyle_Customize);
	}
}

bool QApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void QApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void QApplicationWindow::fullScreenWorkaround() {
	if (myFullScreen) {
		reparent(0, WType_TopLevel, QPoint(0,0));
		reparent(0, WType_TopLevel | WStyle_Customize | WStyle_NoBorderEx, QPoint(0,0));
		show();
	}
}

void QApplicationWindow::close() {
	QMainWindow::close();
}

void QApplicationWindow::setDocument(const QString &fileName) {
	if (!fileName.isEmpty()) {
		application().openFile((const char*)fileName.utf8());
	}
}

ZLViewWidget *QApplicationWindow::createViewWidget() {
	QViewWidget *viewWidget = new QViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	return viewWidget;
}

bool QApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

QtMenuAction::QtMenuAction(ZLApplication &application, const ZLApplication::Menubar::PlainItem &item) : QAction(item.name().c_str(), 0, 0, 0), myApplication(application), myActionId(item.actionId()) {
	connect(this, SIGNAL(activated()), this, SLOT(doSlot()));
}

void QtMenuAction::doSlot() {
	myApplication.doAction(myActionId);
}
