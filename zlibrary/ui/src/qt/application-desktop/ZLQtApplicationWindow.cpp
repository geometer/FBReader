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
#include <qimage.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qlayout.h>
#include <qobjectlist.h>

#include <ZLibrary.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQtApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

class MyIconFactory : public QIconFactory {

public:
	QPixmap *createPixmap(const QIconSet &set, QIconSet::Size size, QIconSet::Mode mode, QIconSet::State state);
};

inline QRgb grayRgb(QRgb rgb) {
	int gray = (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
	return qRgba(gray, gray, gray, qAlpha(rgb) / 2);
}

QPixmap *MyIconFactory::createPixmap(const QIconSet &set, QIconSet::Size size, QIconSet::Mode mode, QIconSet::State state) {
	if (mode != QIconSet::Disabled) {
		return 0;
	}
	QImage image;
	image = set.pixmap(size, QIconSet::Normal, state);
	const int numColors = image.numColors();
	if (numColors > 0) {
		for (int i = 0; i < numColors; ++i) {
			image.setColor(i, grayRgb(image.color(i)));
		}
	} else {
		const int width = image.width();
		const int height = image.height();
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < height; ++j) {
				image.setPixel(i, j, grayRgb(image.pixel(i, j)));
			}
		}
	}

	return new QPixmap(image);
}

ZLQtToolBarAction::ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLApplication::Toolbar::ButtonItem &item) : QAction(parent), myItem(item) {
	static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	QPixmap icon((imagePrefix + myItem.iconName() + ".png").c_str());
	setIconSet(QIconSet(icon));
	QSize size = icon.size();
	QIconSet::setIconSize(QIconSet::Large, size);
	QIconSet::setIconSize(QIconSet::Small, size);
	setToggleAction(item.isToggleButton());
	setToolTip(QString::fromUtf8(myItem.tooltip().c_str()));
	connect(this, SIGNAL(activated()), this, SLOT(onActivated()));
}

void ZLQtToolBarAction::onActivated() {
	((ZLQtApplicationWindow*)parent())->onButtonPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	myActions[&button]->setOn(button.isPressed());
}

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myFullScreen(false),
	myWasMaximized(false),
	myCursorIsHyperlink(false) {

	QIconFactory::installDefaultFactory(new MyIconFactory());

	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	QPixmap icon(iconFileName.c_str());
	setIcon(icon);

	setWFlags(getWFlags() | WStyle_Customize);

	myToolBar = new QToolBar(this);
	myToolBar->boxLayout()->setMargin(5);
	myToolBar->boxLayout()->setSpacing(3);
	setToolBarsMovable(false);

	resize(myWidthOption.value(), myHeightOption.value());
	move(myXOption.value(), myYOption.value());

	qApp->setMainWidget(this);
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
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		ZLApplication::Toolbar::ButtonItem &buttonItem = (ZLApplication::Toolbar::ButtonItem&)*item;
		ZLQtToolBarAction *action = new ZLQtToolBarAction(this, buttonItem);
		action->addTo(myToolBar);
		myActions[&*item] = action;
	} else {
		myToolBar->addSeparator();
		mySeparatorMap[item] = (QWidget*)myToolBar->children()->getLast();
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	QAction *action = myActions[&*item];
	if (action != 0) {
		action->setEnabled(enabled);
		action->setVisible(visible);
	} else {
		QWidget *separator = mySeparatorMap[item];
		if (separator != 0) {
			separator->setShown(visible);
		}
	}
}

void ZLQtApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

ZLQtViewWidgetPositionInfo::ZLQtViewWidgetPositionInfo(const ZLQtApplicationWindow &window) : myWindow(window) {
}

int ZLQtViewWidgetPositionInfo::x() const {
	return 0;
}

int ZLQtViewWidgetPositionInfo::y() const {
	return ((myWindow.myToolBar != 0) && myWindow.myToolBar->isVisible()) ?
		myWindow.myToolBar->height() : 0;
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

void ZLQtApplicationWindow::close() {
	QMainWindow::close();
}

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QMainWindow::setCaption(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	if (hyperlink == myCursorIsHyperlink) {
		return;
	}
	myCursorIsHyperlink = hyperlink;
	if (hyperlink) {
		myStoredCursor = cursor();
		setCursor(Qt::pointingHandCursor);
	} else {
		setCursor(myStoredCursor);
	}
}
