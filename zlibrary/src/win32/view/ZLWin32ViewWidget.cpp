/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLWin32ViewWidget.h"
#include "ZLWin32PaintContext.h"

ZLWin32ViewWidget::ZLWin32ViewWidget(ZLWin32ApplicationWindow &window) : ZLViewWidget((ZLViewWidget::Angle)window.application().AngleStateOption.value()), myWindow(window), myMouseCaptured(false) {
}

void ZLWin32ViewWidget::trackStylus(bool track) {
	myMouseCaptured = track;
}

void ZLWin32ViewWidget::repaint()	{
	RECT rectangle;
	GetClientRect(myWindow.mainWindow(), &rectangle);
	rectangle.top += myWindow.topOffset();
	InvalidateRect(myWindow.mainWindow(), &rectangle, false);
}

void ZLWin32ViewWidget::doPaint()	{
	ZLWin32PaintContext &win32Context = (ZLWin32PaintContext&)view()->context();
	win32Context.beginPaint(myWindow);
	view()->paint();
	win32Context.endPaint();
}
