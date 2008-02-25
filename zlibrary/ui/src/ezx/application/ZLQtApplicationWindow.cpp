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

#include <zapplication.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qwallpaper.h>

#include <ZLibrary.h>

#include "ZLQtApplicationWindow.h"
#include "ZLQtMenuAction.h"
#include "../view/ZLQtViewWidget.h"
#include "../../qt/util/ZLQtKeyUtil.h"
#include "../dialogs/ZLQtUtil.h"
#include "../optionView/ZLQtOptionViewHolder.h"
#include "../../../../core/src/dialogs/ZLOptionView.h"

class MyMenuBar : public QMenuBar, public ZLQtOptionViewHolder {

public:
	MyMenuBar(QWidget *parent, ZLQtApplicationWindow &window) : QMenuBar(parent), myWindow(window), myIndex(-1) {
		QPalette pal(palette());
		pal.setColor(QColorGroup::Button, QColor(77, 77, 77));
		setPalette(pal);
		setMinimumHeight(1);
	}
	~MyMenuBar();
	void setItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);
	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);

private:
	void attachWidget(ZLOptionView &view, QWidget *widget);
	void attachWidgets(ZLOptionView &view, QWidget *widget0, int weight0, QWidget *widget1, int weight1);
	QWidget *widget();
	QWidget *parentWidget();
	void keyPressEvent(QKeyEvent *event) {
		if (event->key() == Key_Escape) {
			QMenuBar::keyPressEvent(event);
		}
	}

private:
	std::map<std::string,ToolBarButton*> myButtons;
	std::map<std::string,int> myActionIndices;
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
		myReleasedPixmap = new QPixmap(
								(ZLibrary::ApplicationImageDirectory()
								+ ZLibrary::FileNameDelimiter
								+ myButton.iconName() + ".png").c_str());
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
	QWallpaper::setAppWallpaperMode(QWallpaper::Off);

	myMainWindow = new MyMainWindow(this);

	myFullScreen = false;
	myTitleHeight = -1;

	myVerticalDelta = -1;

	myToolBar = new MyMenuBar(myMainWindow, *this);
	myMenu = new QPopupMenu(myMainWindow);
	QFont f(qApp->font());
	f.setPointSize(15);
	myMenu->setFont(f);
	myCST = 0;

	qApp->setMainWidget(myMainWindow);
	myMainWindow->show();
	myMainWindow->setWFlags(myMainWindow->getWFlags() | QObject::WStyle_Customize);
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	delete myMainWindow;
}

MyMenuBar::~MyMenuBar() {
	for (std::map<std::string,ToolBarButton*>::iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		delete it->second;
	}
}

void MyMenuBar::attachWidget(ZLOptionView&, QWidget *widget) {
	insertItem(widget, 10001, myIndex++);
}

void MyMenuBar::attachWidgets(ZLOptionView&, QWidget *widget0, int, QWidget *widget1, int) {
	insertItem(widget0, 10001, myIndex++);
	insertItem(widget1, 10001, myIndex++);
}

QWidget *MyMenuBar::widget() {
	return this;
}

QWidget *MyMenuBar::parentWidget() {
	return 0;
}

void ZLQtApplicationWindow::setCaption(const std::string & /* caption */) {
}

void ZLQtApplicationWindow::grabAllKeys(bool /* grab */) {
#if 0
	if (grab) {
		ZApplication::grabKeyboard();
	} else {
		ZApplication::ungrabKeyboard();
	}
#endif
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
	QFont f(qApp->font());
	f.setPointSize(15);
	qmenu->setFont(f);
	myMenuStack.top()->insertItem(::qtString(submenu.menuName()), qmenu);
	myMenuStack.push(qmenu);
}

void ZLQtApplicationWindow::MenuUpdater::processSubmenuAfterItems(ZLApplication::Menubar::Submenu&) {
	myMenuStack.pop();
}

void ZLQtApplicationWindow::MenuUpdater::processItem(ZLApplication::Menubar::PlainItem &item) {
	if (myWindow.myMenuMask[myCounter++]) {
		const std::string &id = item.actionId();
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
	switch (item->type()) {
		case ZLApplication::Toolbar::Item::BUTTON:
			{
				ZLApplication::Toolbar::ButtonItem &button = (ZLApplication::Toolbar::ButtonItem&)*item;
				const std::string &id = button.actionId();
				std::map<std::string,int> ::const_iterator iter = myActionIndices.find(id);
				int actionIndex = (iter != myActionIndices.end()) ? iter->second : 0;
				if (visible) {
					if ((actionIndex == 0) || (idAt(myIndex) != actionIndex)) {
						ToolBarButton *tbButton = myButtons[id];
						if (actionIndex == 0) {
							tbButton = new ToolBarButton(myWindow, button);
							myButtons[id] = tbButton;
							actionIndex = myActionIndices.size() + 1;
							myActionIndices[id] = actionIndex;
						}
						insertItem(tbButton->pixmap(), tbButton, SLOT(doActionSlot()), 0, actionIndex, myIndex);
					}
					setItemEnabled(actionIndex, enabled);
					++myIndex;
				} else {
					if ((actionIndex != 0) && (idAt(myIndex) == actionIndex)) {
						removeItem(actionIndex);
					}
				}
			}
			break;
		case ZLApplication::Toolbar::Item::OPTION_ENTRY:
			if (idAt(myIndex) < 10001) {
				shared_ptr<ZLOptionEntry> entry = ((ZLApplication::Toolbar::OptionEntryItem&)*item).entry();
				if (entry->isVisible()) {
					ZLOptionView *view = createViewByEntry("", "", entry);
					view->setVisible(true);
				}
			} else {
				++myIndex;
			}
			break;
		case ZLApplication::Toolbar::Item::SEPARATOR:
			break;
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	((MyMenuBar*)myToolBar)->setItemState(item, visible, enabled);
}

static const QString CLOSE_BUTTON = "CST_Exit";
static const QString BACK_BUTTON = "CST_Back";

void ZLQtApplicationWindow::refresh() {
	const QString &rightButtonResource =
		application().isViewFinal() ? CLOSE_BUTTON : BACK_BUTTON;
	if (myRightButtonResource != rightButtonResource) {
		myRightButtonResource = rightButtonResource;
		myCST->getRightBtn()->setResourceID(myRightButtonResource);
	}

	if (((MyMenuBar*)myToolBar)->myIndex == -1) {
		((MyMenuBar*)myToolBar)->myIndex = 0;
		ZLApplicationWindow::refresh();
		((MyMenuBar*)myToolBar)->myIndex = -1;
	}

	MenuMaskCalculator calculator(*this);
	calculator.processMenu(application());
	if (calculator.shouldBeUpdated()) {
		myMenu->clear();
		MenuUpdater(*this).processMenu(application());
	} else {
		for (std::map<std::string,ZLQtMenuAction*>::iterator it = myMenuMap.begin(); it != myMenuMap.end(); ++it) {
			it->second->setEnabled(application().isActionEnabled(it->first));
		}
	}
}

void ToolBarButton::doActionSlot() {
	myWindow.onButtonPress(myButton);
}

void MyMainWindow::keyPressEvent(QKeyEvent *event) {
	if (event->key() == 0x1031) {
		QMainWindow::keyPressEvent(event);
	} else {
		myApplicationWindow->application().doActionByKey(ZLQtKeyUtil::keyName(event));
	}
}

void MyMainWindow::focusInEvent(QFocusEvent*) {
	if (myApplicationWindow == 0) return;
	if (myIsDeactivated) {
		myApplicationWindow->myCST->hide();
		myApplicationWindow->myCST->show();
		myIsDeactivated = false;
	}
}

void MyMainWindow::focusOutEvent(QFocusEvent *) {
	if (myApplicationWindow == 0) return;
	if (myApplicationWindow->isFullscreen()) {
		myApplicationWindow->setFullscreen(false);
		myIsDeactivated = true;
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
		myCST->hide();
		myMainWindow->showFullScreen();
	} else {
		myToolBar->show();
		myCST->show();
		myMainWindow->showNormal();
	}
}

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void MyMainWindow::closeEvent(QCloseEvent *event) {
	if (myApplicationWindow != 0) {
		if (myApplicationWindow->application().closeView()) {
			myApplicationWindow = 0;
			event->accept();
		} else {
			event->ignore();
		}
	}
}

void MyMainWindow::doActionQuitSlot() {
	myApplicationWindow->application().closeView();
}

void MyMainWindow::setFullscreenSlot() {
	myApplicationWindow->setFullscreen(true);
}

void MyMainWindow::connectCSTButtons(UTIL_CST *cst) {
	connect(cst->getMidBtn(), SIGNAL(clicked()), this, SLOT(setFullscreenSlot()));
	connect(cst->getRightBtn(), SIGNAL(clicked()), this, SLOT(doActionQuitSlot()));
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
	QWidget *main = new QWidget(myMainWindow);
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(main, *this);

	QVBoxLayout *layout = new QVBoxLayout(main);
	layout->addWidget(viewWidget->widget(), 1);

	myRightButtonResource = CLOSE_BUTTON;
	myCST = new UTIL_CST(main, ZLibrary::ApplicationName().c_str(), "CST_Menu", myRightButtonResource);
	myCST->setFixedSize(ZGlobal::getCstR().size());
	layout->addWidget(myCST);
	myCST->getLeftBtn()->setPopup(myMenu);
	myMainWindow->connectCSTButtons(myCST);

	myMainWindow->setCentralWidget(main);
	return viewWidget;
}

ZLQtMenuAction::ZLQtMenuAction(ZLQtApplicationWindow &window, const ZLApplication::Menubar::PlainItem &item) : QAction(::qtString(item.name()), 0, 0, 0), myWindow(window), myActionId(item.actionId()) {
	connect(this, SIGNAL(activated()), this, SLOT(doSlot()));
}

void ZLQtMenuAction::doSlot() {
	myWindow.application().doAction(myActionId);
	myWindow.myMainWindow->setFocus();
}

void MyMenuBar::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	const std::string &actionId = button.actionId();
	ToolBarButton *tbButton = myButtons[actionId];
	if ((tbButton != 0) && (tbButton->toggle())) {
		changeItem(myActionIndices[actionId], tbButton->pixmap());
	}
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	((MyMenuBar*)myToolBar)->setToggleButtonState(button);
}

QMainWindow *ZLQtApplicationWindow::mainWindow() {
	return myMainWindow;
}
