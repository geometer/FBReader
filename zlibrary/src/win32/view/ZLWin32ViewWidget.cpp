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

#include <iostream>

#include "ZLWin32ViewWidget.h"
#include "ZLWin32PaintContext.h"

ZLWin32ViewWidget::ZLWin32ViewWidget(ZLApplication &application, HWND window) : ZLViewWidget((ZLViewWidget::Angle)application.AngleStateOption.value()), myApplication(application), myWindow(window) {
}

void ZLWin32ViewWidget::trackStylus(bool track) {
}

void ZLWin32ViewWidget::repaint()	{
	InvalidateRect(myWindow, 0, false);
}

void ZLWin32ViewWidget::doPaint()	{
	ZLWin32PaintContext &win32Context = (ZLWin32PaintContext&)view()->context();
	win32Context.beginPaint(myWindow);
	view()->paint();
	win32Context.endPaint();
}
