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
#include <qpainter.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>

#include "QApplicationWindow.h"
#include "QtMenuAction.h"
#include "../view/QViewWidget.h"
#include "../dialogs/QDialogManager.h"
#include "../../qt/util/QKeyUtil.h"

class MyMenuBar : public QPEMenuBar {

public:
	MyMenuBar(QWidget *parent) : QPEMenuBar(parent), myIndex(-1) {}
	~MyMenuBar();
	void setItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);
	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);

private:
	void keyPressEvent(QKeyEvent *event) {
		if (event->key() == Key_Escape) {
			QPEMenuBar::keyPressEvent(event);
		}
	}

private:
	std::map<int,ToolBarButton*> myButtons;

public:
	int myIndex;
};

ToolBarButton::ToolBarButton(QApplicationWindow &window, ZLApplication::Toolbar::ButtonItem &button) : myWindow(window), myButton(button), myReleasedPixmap(0), myPressedPixmap(0), myIsPressed(button.isPressed()) {
}

ToolBarButton::~ToolBarButton() {
	if (myReleasedPixmap != 0) {
		delete myReleasedPixmap;
	}
	if (myPressedPixmap != 0) {
		delete myPressedPixmap;
	}
}

QPixmap &ToolBarButton::pixmap() {
	if (myReleasedPixmap == 0) {
		myReleasedPixmap = new QPixmap(Resource::loadPixmap((ZLApplication::ApplicationName() + '/' + myButton.iconName()).c_str()));
	}
	if (myButton.isPressed()) {
		if (myPressedPixmap == 0) {
			const int w = myReleasedPixmap->width();
			const int h = myReleasedPixmap->height();
			myPressedPixmap = new QPixmap(w, h);		
			myPressedPixmap->fill(Qt::white);
			QPainter painter(myPressedPixmap);
			painter.drawPixmap(0, 0, *myReleasedPixmap);
			painter.setPen(Qt::black);
			painter.drawRect(0, 0, w, h);
		}
		return *myPressedPixmap;
	} else {
		return *myReleasedPixmap;
	}
}

bool ToolBarButton::toggle() {
	if (myIsPressed != myButton.isPressed()) {
		myIsPressed = !myIsPressed;
		return true;
	}
	return false;
}

QApplicationWindow::QApplicationWindow(ZLApplication *a) : ZLApplicationWindow(a) {
	setWFlags(getWFlags() | WStyle_Customize);

	myFullScreen = false;
	myTitleHeight = -1;

	myVerticalDelta = -1;

	myToolBar = new MyMenuBar(this);
	myMenu = new QPopupMenu(myToolBar);
	myToolBar->insertItem(QString::null, myMenu, -1, 0);

	((QPEApplication*)qApp)->showMainWidget(this);
}

MyMenuBar::~MyMenuBar() {
	for (std::map<int,ToolBarButton*>::iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		delete it->second;
	}
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
			action = new QtMenuAction(myWindow, item);
			myWindow.myMenuMap[id] = action;
		}
		action->addTo(myMenuStack.top());
		action->setEnabled(myWindow.application().isActionEnabled(id));
	}
}

void QApplicationWindow::MenuUpdater::processSepartor(ZLApplication::Menubar::Separator&) {
	myMenuStack.top()->insertSeparator();
}

void MyMenuBar::setItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	if (item->isButton()) {
		ZLApplication::Toolbar::ButtonItem &button = (ZLApplication::Toolbar::ButtonItem&)*item;
		int id = button.actionId();
		if (visible) {
			if (idAt(myIndex) != id) {
				ToolBarButton *tbButton = myButtons[id];
				if (tbButton == 0) {
					tbButton = new ToolBarButton(*(QApplicationWindow*)parent(), button);
					myButtons[id] = tbButton;
				}
				insertItem(tbButton->pixmap(), tbButton, SLOT(doActionSlot()), 0, id, myIndex);
			}
			setItemEnabled(id, enabled);
			++myIndex;
		} else {
			if (idAt(myIndex) == id) {
				removeItem(id);
			}
		}
	}
}

void QApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	((MyMenuBar*)myToolBar)->setItemState(item, visible, enabled);
}

void QApplicationWindow::refresh() {
	if (((MyMenuBar*)myToolBar)->myIndex == -1) {
		((MyMenuBar*)myToolBar)->myIndex = 1;
		ZLApplicationWindow::refresh();
		((MyMenuBar*)myToolBar)->myIndex = -1;
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

void ToolBarButton::doActionSlot() {
	myWindow.onButtonPress(myButton);
}

void QApplicationWindow::keyPressEvent(QKeyEvent *event) {
	application().doActionByKey(QKeyUtil::keyName(event));
}

void QApplicationWindow::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
	fullScreenWorkaround();
}

int QApplicationWindow::veritcalAdjustment() {
	if (myFullScreen || (myVerticalDelta == -1)) {
		return 0;
	}
	return qApp->desktop()->height() - myVerticalDelta - height();
}

void QApplicationWindow::resizeEvent(QResizeEvent *event) {
	if (event->size().width() != qApp->desktop()->width()) {
		QSize oldSize = event->size();
		QSize newSize = oldSize;
		newSize.setWidth(qApp->desktop()->width());
		QResizeEvent newEvent(newSize, oldSize);
		QApplication::sendEvent(this, &newEvent);
		return;
	}
	if ((myVerticalDelta == -1) && !myFullScreen) {
		myVerticalDelta = qApp->desktop()->height() - event->size().height();
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
		showMaximized();
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

bool QApplicationWindow::isMousePresented() const {
	return false;
}

bool QApplicationWindow::isKeyboardPresented() const {
	// TODO: implement
	return true;
}

QtMenuAction::QtMenuAction(QApplicationWindow &window, const ZLApplication::Menubar::PlainItem &item) : QAction(item.name().c_str(), 0, 0, 0), myWindow(window), myActionId(item.actionId()) {
	connect(this, SIGNAL(activated()), this, SLOT(doSlot()));
}

void QtMenuAction::doSlot() {
	myWindow.application().doAction(myActionId);
	myWindow.setFocus();
}

void MyMenuBar::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	const int actionId = button.actionId();
	ToolBarButton *tbButton = myButtons[actionId];
	if ((tbButton != 0) && (tbButton->toggle())) {
		changeItem(actionId, tbButton->pixmap());
	}
}

void QApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	((MyMenuBar*)myToolBar)->setToggleButtonState(button);
}
