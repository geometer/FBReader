/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QDesktopWidget>
#include <QtGui/QToolBar>

#include "ZLQtMainWindow.h"

static const std::string OPTIONS = "Options";

ZLQtMainWindow::ZLQtMainWindow(QWidget *parent, const std::string &windowName) :
	QMainWindow(parent),
	myOptionPrefix(windowName.empty() ? windowName : windowName + "_"),
	myWindowStateOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, myOptionPrefix + "WindowSizeState", NORMAL),
	myXOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, myOptionPrefix + "XPosition", 0, QApplication::desktop()->width() - 10, 10),
	myYOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, myOptionPrefix + "YPosition", 0, QApplication::desktop()->height() - 10, 10),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, myOptionPrefix + "Width", 10, QApplication::desktop()->width(), QApplication::desktop()->width() / 2),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, myOptionPrefix + "Height", 10, QApplication::desktop()->height(), QApplication::desktop()->height() / 2)
	{
	setUnifiedTitleAndToolBarOnMac(true);
	myToolbar = new QToolBar(this);
	myToolbar->setFocusPolicy(Qt::NoFocus);
	myToolbar->setMovable(false);
	addToolBar(myToolbar);

	const int x = myXOption.value();
	const int y = myYOption.value();
	const int w = myWidthOption.value();
	const int h = myHeightOption.value();
	setGeometry(x, y, w, h);
	QPoint position = pos();
	const int deltaX = x - pos().x();
	const int deltaY = y - pos().y();
	const int deltaW = w - width();
	const int deltaH = h - height();
	setGeometry(x + deltaX, y + deltaY, w + deltaW, h + deltaH);
}

ZLQtMainWindow::~ZLQtMainWindow() {
	saveWindowGeometry();
}

void ZLQtMainWindow::resizeEvent(QResizeEvent* event) {
	QMainWindow::resizeEvent(event);
	if (isFullScreen()) {
		myToolbar->hide();
	} else {
		myToolbar->show();
	}
	saveWindowGeometry();
}

void ZLQtMainWindow::showWithGeometry() {
	switch (myWindowStateOption.value()) {
		default:
		case NORMAL:
			show();
			break;
		case FULLSCREEN:
			showFullScreen();
			break;
		case MAXIMIZED:
			showMaximized();
			break;
	}
}

void ZLQtMainWindow::saveWindowGeometry() {
	if (isFullScreen()) {
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
}
