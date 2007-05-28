/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLQtApplicationWindow.h"
#include "ZLQtMenuAction.h"
#include "../view/ZLQtViewWidget.h"
#include "../../qt/util/ZLQtKeyUtil.h"
#include "../dialogs/ZLQtUtil.h"

class MyMenuBar : public QPEMenuBar {

public:
	MyMenuBar(QWidget *parent, ZLQtApplicationWindow &window) : QPEMenuBar(parent), myWindow(window), myIndex(-1) {}
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
	ZLQtApplicationWindow &myWindow;

public:
	int myIndex;
};

ToolBarButton::ToolBarButton(ZLQtApplicationWindow &window, ZLApplication::Toolbar::ButtonItem &button) : myWindow(window), myButton(button), myReleasedPixmap(0), myPressedPixmap(0), myIsPressed(button.isPressed()) {
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
		myReleasedPixmap = new QPixmap(Resource::loadPixmap((ZLApplication::ApplicationName() + ZLApplication::FileNameDelimiter + myButton.iconName()).c_str()));
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

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *a) : ZLApplicationWindow(a) {
	myMainWindow = new MyMainWindow(this);

	myFullScreen = false;
	myTitleHeight = -1;

	myVerticalDelta = -1;

	myToolBar = new MyMenuBar(myMainWindow, *this);
	myMenu = new QPopupMenu(myToolBar);
	myToolBar->insertItem(QString::null, myMenu, -1, 0);

	((QPEApplication*)qApp)->showMainWidget(myMainWindow);
	myMainWindow->setWFlags(myMainWindow->getWFlags() | QObject::WStyle_Customize);
}

MyMenuBar::~MyMenuBar() {
	for (std::map<int,ToolBarButton*>::iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		delete it->second;
	}
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QString qCaption = ::qtString(caption);
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	myMainWindow->setCaption(qCaption);
}

bool ZLQtApplicationWindow::isFullKeyboardControlSupported() const {
	return true;
}

void ZLQtApplicationWindow::grabAllKeys(bool grab) {
	if (grab) {
		QPEApplication::grabKeyboard();
	} else {
		QPEApplication::ungrabKeyboard();
	}
}

void ZLQtApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr) {
}

void ZLQtApplicationWindow::initMenu() {
}

ZLQtApplicationWindow::MenuMaskCalculator::MenuMaskCalculator(ZLQtApplicationWindow &window) : myWindow(window), myFirstTime(myWindow.myMenuMask.empty()), myShouldBeUpdated(myFirstTime), myCounter(0) {
}

void ZLQtApplicationWindow::MenuMaskCalculator::processSubmenuBeforeItems(ZLApplication::Menubar::Submenu&) {
}

void ZLQtApplicationWindow::MenuMaskCalculator::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
}

void ZLQtApplicationWindow::MenuMaskCalculator::processSepartor(ZLApplication::Menubar::Separator&) {
}

void ZLQtApplicationWindow::MenuMaskCalculator::processItem(ZLApplication::Menubar::PlainItem &item) {
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

ZLQtApplicationWindow::MenuUpdater::MenuUpdater(ZLQtApplicationWindow &window) : myWindow(window), myCounter(0) {
	myMenuStack.push(myWindow.myMenu);
}

void ZLQtApplicationWindow::MenuUpdater::processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu) {
	QPopupMenu *qmenu = new QPopupMenu(myMenuStack.top());
	myMenuStack.top()->insertItem(::qtString(submenu.menuName()), qmenu);
	myMenuStack.push(qmenu);
}

void ZLQtApplicationWindow::MenuUpdater::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
	myMenuStack.pop();
}

void ZLQtApplicationWindow::MenuUpdater::processItem(ZLApplication::Menubar::PlainItem &item) {
	if (myWindow.myMenuMask[myCounter++]) {
		const int id = item.actionId();
		ZLQtMenuAction *action = myWindow.myMenuMap[id];
		if (action == 0) {
			action = new ZLQtMenuAction(myWindow, item);
			myWindow.myMenuMap[id] = action;
		}
		action->addTo(myMenuStack.top());
		action->setEnabled(myWindow.application().isActionEnabled(id));
	}
}

void ZLQtApplicationWindow::MenuUpdater::processSepartor(ZLApplication::Menubar::Separator&) {
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
					tbButton = new ToolBarButton(myWindow, button);
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

void ZLQtApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	((MyMenuBar*)myToolBar)->setItemState(item, visible, enabled);
}

void ZLQtApplicationWindow::refresh() {
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
		for (std::map<int,ZLQtMenuAction*>::iterator it = myMenuMap.begin(); it != myMenuMap.end(); ++it) {
			it->second->setEnabled(application().isActionEnabled(it->first));
		}
	}
}

void ToolBarButton::doActionSlot() {
	myWindow.onButtonPress(myButton);
}

void MyMainWindow::keyPressEvent(QKeyEvent *event) {
	myApplicationWindow->application().doActionByKey(ZLQtKeyUtil::keyName(event));
}

void MyMainWindow::focusInEvent(QFocusEvent*) {
	if (myApplicationWindow != 0) {
		if (myApplicationWindow->myFullScreen && (size() != qApp->desktop()->size())) {
			showNormal();
			showFullScreen();
		}
		myApplicationWindow->fullScreenWorkaround();
	}
}

int ZLQtApplicationWindow::verticalAdjustment() {
	if (myFullScreen || (myVerticalDelta == -1)) {
		return 0;
	}
	return qApp->desktop()->height() - myVerticalDelta - myMainWindow->height();
}

void MyMainWindow::resizeEvent(QResizeEvent *event) {
	if (event->size().width() != qApp->desktop()->width()) {
		QSize oldSize = event->size();
		QSize newSize = oldSize;
		newSize.setWidth(qApp->desktop()->width());
		QResizeEvent newEvent(newSize, oldSize);
		QApplication::sendEvent(this, &newEvent);
		return;
	}
	if ((myApplicationWindow->myVerticalDelta == -1) && !myApplicationWindow->myFullScreen) {
		myApplicationWindow->myVerticalDelta = qApp->desktop()->height() - event->size().height();
	}
	if (myApplicationWindow->myFullScreen && (size() != qApp->desktop()->size())) {
		int titleHeight = topData()->normalGeometry.top();
		if (titleHeight > 0) {
			myApplicationWindow->myTitleHeight = titleHeight;
		}
		topData()->normalGeometry = QRect(0, 0, -1, -1);
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
		myToolBar->hide();
		myMainWindow->showFullScreen();
	} else {
		myToolBar->show();
		myMainWindow->showNormal();
		if (myTitleHeight > 0) {
			myMainWindow->move(1, myTitleHeight);
			myTitleHeight = -1;
		}
		myMainWindow->setWFlags(myMainWindow->getWFlags() | QObject::WStyle_Customize);
		myMainWindow->showMaximized();
	}
}

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void MyMainWindow::closeEvent(QCloseEvent *event) {
	if (myApplicationWindow != 0) {
		if (myApplicationWindow->application().closeView()) {
			delete &myApplicationWindow->application();
			myApplicationWindow = 0;
			event->accept();
		} else {
			event->ignore();
		}
	}
}

void ZLQtApplicationWindow::fullScreenWorkaround() {
	if (myFullScreen) {
		myMainWindow->reparent(0, QObject::WType_TopLevel, QPoint(0,0));
		myMainWindow->reparent(0, QObject::WType_TopLevel | QObject::WStyle_Customize | QObject::WStyle_NoBorderEx, QPoint(0,0));
		myMainWindow->show();
	}
}

void ZLQtApplicationWindow::close() {
	myMainWindow->close();
}

void MyMainWindow::setDocument(const QString &fileName) {
	if (!fileName.isEmpty()) {
		myApplicationWindow->application().openFile((const char*)fileName.utf8());
	}
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(myMainWindow, *this);
	myMainWindow->setCentralWidget(viewWidget->widget());
	return viewWidget;
}

bool ZLQtApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

bool ZLQtApplicationWindow::isMousePresented() const {
	return false;
}

bool ZLQtApplicationWindow::isKeyboardPresented() const {
	// TODO: implement
	return true;
}

ZLQtMenuAction::ZLQtMenuAction(ZLQtApplicationWindow &window, const ZLApplication::Menubar::PlainItem &item) : QAction(::qtString(item.name()), 0, 0, 0), myWindow(window), myActionId(item.actionId()) {
	connect(this, SIGNAL(activated()), this, SLOT(doSlot()));
}

void ZLQtMenuAction::doSlot() {
	myWindow.application().doAction(myActionId);
	myWindow.myMainWindow->setFocus();
}

void MyMenuBar::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	const int actionId = button.actionId();
	ToolBarButton *tbButton = myButtons[actionId];
	if ((tbButton != 0) && (tbButton->toggle())) {
		changeItem(actionId, tbButton->pixmap());
	}
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	((MyMenuBar*)myToolBar)->setToggleButtonState(button);
}

QMainWindow *ZLQtApplicationWindow::mainWindow() {
	return myMainWindow;
}
