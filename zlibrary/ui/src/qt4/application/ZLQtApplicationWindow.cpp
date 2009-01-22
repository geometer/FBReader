/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>
#include <QtGui/QWheelEvent>
#include <QtGui/QDockWidget>
#include <QtCore/QObjectList>

#include <ZLibrary.h>
#include <ZLPopupData.h>

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
	QString text = QString::fromUtf8(myItem.tooltip().c_str());
	setText(text);
	setToolTip(text);
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
	myFullscreenToolBar(0),
	myDocWidget(0),
	myFullScreen(false),
	myWasMaximized(false),
	myCursorIsHyperlink(false) {

	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	QPixmap icon(iconFileName.c_str());
	setWindowIcon(icon);

	myWindowToolBar = new QToolBar(this);
	myWindowToolBar->setFocusPolicy(Qt::NoFocus);
	myWindowToolBar->setMovable(false);
	addToolBar(myWindowToolBar);
	myWindowToolBar->setIconSize(QSize(32, 32));

	if (hasFullscreenToolbar()) {
		myFullscreenToolBar = new QToolBar();
		myFullscreenToolBar->setMovable(false);
		myFullscreenToolBar->setIconSize(QSize(32, 32));
		myFullscreenToolBar->hide();
	}

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
		myWindowToolBar->hide();
		showFullScreen();
		if (myFullscreenToolBar != 0) {
			if (myDocWidget == 0) {
				myDocWidget = new QDockWidget(this);
				myDocWidget->setWidget(myFullscreenToolBar);
				myDocWidget->setFloating(true);
				myDocWidget->setAllowedAreas(Qt::NoDockWidgetArea);
			}
			myDocWidget->show();
			myFullscreenToolBar->show();
			myDocWidget->setMinimumSize(myDocWidget->size());
			myDocWidget->setMaximumSize(myDocWidget->size());
		}
	} else {
		myWindowToolBar->show();
		showNormal();
		if (myWasMaximized) {
			showMaximized();
		}
		if (myDocWidget != 0) {
			//myFullscreenToolBar->hide();
			myDocWidget->hide();
		}
	}
}

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLQtApplicationWindow::keyReleaseEvent(QKeyEvent *event) {
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
	QToolBar *tb = toolbar(type(*item));
	QAction *action = 0;

	switch (item->type()) {
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
			action = new ZLQtToolBarAction(this, (ZLToolbar::AbstractButtonItem&)*item);
			tb->addAction(action);
			break;
		case ZLToolbar::Item::MENU_BUTTON:
		{
			ZLToolbar::MenuButtonItem &buttonItem = (ZLToolbar::MenuButtonItem&)*item;
			QToolButton *button = new QToolButton(tb);
			button->setFocusPolicy(Qt::NoFocus);
			button->setDefaultAction(new ZLQtToolBarAction(this, buttonItem));
			button->setMenu(new QMenu(button));
			button->setPopupMode(QToolButton::MenuButtonPopup);
			action = tb->addWidget(button);
			myMenuButtons[&buttonItem] = button;
			shared_ptr<ZLPopupData> popupData = buttonItem.popupData();
			myPopupIdMap[&buttonItem] =
				popupData.isNull() ? (size_t)-1 : (popupData->id() - 1);
			break;
		}
		case ZLToolbar::Item::TEXT_FIELD:
		{
			ZLToolbar::TextFieldItem &textFieldItem =
				(ZLToolbar::TextFieldItem&)*item;
			LineEditParameter *para = new LineEditParameter(tb, *this, textFieldItem);
			addVisualParameter(textFieldItem.parameterId(), para);
			action = para->action();
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			action = tb->addSeparator();
			break;
	}

	if (action != 0) {
		myActions[&*item] = action;
	}
}

ZLQtRunPopupAction::ZLQtRunPopupAction(QObject *parent, shared_ptr<ZLPopupData> data, size_t index) : QAction(parent), myData(data), myIndex(index) {
	setText(QString::fromUtf8(myData->text(myIndex).c_str()));
	connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

ZLQtRunPopupAction::~ZLQtRunPopupAction() {
}

void ZLQtRunPopupAction::onActivated() {
	myData->run(myIndex);
}

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	QAction *action = myActions[&*item];
	if (action != 0) {
		action->setEnabled(enabled);
		action->setVisible(visible);
	}
	switch (item->type()) {
		default:
			break;
		case ZLToolbar::Item::MENU_BUTTON:
		{
			ZLToolbar::MenuButtonItem &buttonItem = (ZLToolbar::MenuButtonItem&)*item;
			shared_ptr<ZLPopupData> data = buttonItem.popupData();
			if (!data.isNull() && (data->id() != myPopupIdMap[&buttonItem])) {
				myPopupIdMap[&buttonItem] = data->id();
				QToolButton *button = myMenuButtons[&buttonItem];
				QMenu *menu = button->menu();
				menu->clear();
				const size_t count = data->count();
				for (size_t i = 0; i < count; ++i) {
					menu->addAction(new ZLQtRunPopupAction(menu, data, i));
				}
			}
			break;
		}
	}
}

void ZLQtApplicationWindow::processAllEvents() {
	qApp->processEvents();
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

void ZLQtApplicationWindow::setFocusToMainWidget() {
	centralWidget()->setFocus();
}
