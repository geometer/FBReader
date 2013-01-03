/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QToolBar>

#include "ZLQtMainWindow.h"

ZLQtMainWindow::ZLQtMainWindow(QWidget *parent, const std::string &windowName) :
	QMainWindow(parent),
	myOptionPrefix(windowName.empty() ? windowName : windowName + "_"),
	myWindowStateOption(ZLCategoryKey::LOOK_AND_FEEL, "Options", myOptionPrefix + "WindowSizeState", NORMAL),
	myGeometryOptions(myOptionPrefix)
	{
	setUnifiedTitleAndToolBarOnMac(true);
	myToolbar = new QToolBar(this);
	myToolbar->setFocusPolicy(Qt::NoFocus);
	myToolbar->setMovable(false);
	addToolBar(myToolbar);

	myGeometryOptions.setToWidget(*this);
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
		myGeometryOptions.getFromWidget(*this);
	}
}
