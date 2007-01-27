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
	HWND window = myWindow.mainWindow();
	int offset = myWindow.topOffset();

	RECT rectangle;
	GetClientRect(window, &rectangle);
	const int width = rectangle.right - rectangle.left + 1;
	const int height = rectangle.bottom - rectangle.top + 1 - offset;

	win32Context.updateInfo(window, width, height);
	view()->paint();
	PAINTSTRUCT paintStructure;
	HDC dc = BeginPaint(window, &paintStructure);
	BitBlt(dc, 0, offset, width, height, win32Context.displayContext(), 0, 0, SRCCOPY);
	EndPaint(window, &paintStructure);
}

void ZLWin32ViewWidget::onMousePress(int x, int y) {
	ZLPaintContext &context = view()->context();
	x -= context.leftMargin();
	y -= context.topMargin();
	view()->onStylusPress(x, y);
}

void ZLWin32ViewWidget::onMouseRelease(int x, int y) {
	ZLPaintContext &context = view()->context();
	x -= context.leftMargin();
	y -= context.topMargin();
	view()->onStylusRelease(x, y);
}

void ZLWin32ViewWidget::onMouseMove(int x, int y) {
	ZLPaintContext &context = view()->context();
	x -= context.leftMargin();
	y -= context.topMargin();
	view()->onStylusMove(x, y);
}

void ZLWin32ViewWidget::onMouseMovePressed(int x, int y) {
	ZLPaintContext &context = view()->context();
	x -= context.leftMargin();
	y -= context.topMargin();
	if (x < 0) {
		x = 0;
	} else if (x >= context.width()) {
		x = context.width() - 1;
	}
	if (y < 0) {
		y = 0;
	} else if (y >= context.height()) {
		y = context.height() - 1;
	}
	view()->onStylusMovePressed(x, y);
}
