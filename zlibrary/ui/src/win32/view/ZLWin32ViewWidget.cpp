/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

ZLWin32ViewWidget::ZLWin32ViewWidget(ZLWin32ApplicationWindow &window) : ZLViewWidget((ZLViewWidget::Angle)window.application().AngleStateOption.value()), myWindow(window), myMouseCaptured(false), myRotator(0) {
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

ZLWin32ViewWidget::Rotator::Rotator(int width, int height) : myWidth(width), myHeight(height), mySign(-1) {
	int size = width * height;
	myBuffer = new long[size];
	myBuffer1 = new long[size];
}

ZLWin32ViewWidget::Rotator::~Rotator() {
	delete[] myBuffer;
	delete[] myBuffer1;
}

void ZLWin32ViewWidget::Rotator::setSize(int width, int height) {
	int size = width * height;
	if (size != myWidth * myHeight) {
		delete[] myBuffer;
		delete[] myBuffer1;
		myBuffer = new long[size];
		myBuffer1 = new long[size];
	}
	myWidth = width;
	myHeight = height;
}

void ZLWin32ViewWidget::Rotator::setRotation(bool counterClockWise) {
	mySign = counterClockWise ? -1 : 1;
}

void ZLWin32ViewWidget::Rotator::retrieve(HDC dc, HBITMAP bitmap) {
	BITMAPINFO bi;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = myHeight;
	bi.bmiHeader.biHeight = mySign * myWidth;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 4 * myWidth * myHeight;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	GetDIBits(dc, bitmap, 0, myWidth, myBuffer, &bi, DIB_RGB_COLORS);
}

void ZLWin32ViewWidget::Rotator::rotate() {
	for (int i = 0; i < myHeight; ++i) {
		for (int j = 0; j < myWidth; ++j) {
			myBuffer1[i * myWidth + j] = myBuffer[j * myHeight + i];
		}
	}
}

void ZLWin32ViewWidget::Rotator::draw(HDC dc, int topOffset) {
	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
  bi.bmiHeader.biWidth = myWidth;
	bi.bmiHeader.biHeight = -mySign * myHeight;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = 4 * myWidth * myHeight;
  bi.bmiHeader.biClrUsed = 0;
  bi.bmiHeader.biClrImportant = 0;
	int offset = (mySign == 1) ? topOffset : topOffset - 1;
	SetDIBitsToDevice(dc, 0, offset, myWidth, myHeight, 0, 0, 0, myHeight, myBuffer1, &bi, DIB_RGB_COLORS);
}

void ZLWin32ViewWidget::doPaint()	{
	ZLWin32PaintContext &win32Context = (ZLWin32PaintContext&)view()->context();
	HWND window = myWindow.mainWindow();
	int offset = myWindow.topOffset();

	RECT rectangle;
	GetClientRect(window, &rectangle);
	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top - offset;

	switch (rotation()) {
		case DEGREES0:
		case DEGREES180:
			win32Context.updateInfo(window, width, height);
			break;
		case DEGREES90:
		case DEGREES270:
			win32Context.updateInfo(window, height, width);
			break;
	}
	view()->paint();

	PAINTSTRUCT paintStructure;
	HDC dc = BeginPaint(window, &paintStructure);
	switch (rotation()) {
		case DEGREES0:
			if (myRotator != 0) {
				delete myRotator;
				myRotator = 0;
			}
			SetMapMode(dc, MM_TEXT);
			BitBlt(dc, 0, offset, width + 1, height + 1, win32Context.displayContext(), 0, 0, SRCCOPY);
			break;
		case DEGREES180:
		{
			if (myRotator != 0) {
				delete myRotator;
				myRotator = 0;
			}
			SetMapMode(dc, MM_ANISOTROPIC);
			SIZE size;
			GetViewportExtEx(dc, &size);
			SetViewportExtEx(dc, -size.cx, -size.cy, 0);
			SetViewportOrgEx(dc, width - 1, height + 2 * offset - 1, 0);
			BitBlt(dc, 0, offset, width + 1, height + 1, win32Context.displayContext(), 0, 0, SRCCOPY);
			break;
		}
		case DEGREES90:
		case DEGREES270:
			if (myRotator == 0) {
				myRotator = new Rotator(width + 1, height + 1);
			} else {
				myRotator->setSize(width + 1, height + 1);
			}

			myRotator->setRotation(rotation() == DEGREES90);
			myRotator->retrieve(win32Context.displayContext(), win32Context.buffer());
			myRotator->rotate();
			myRotator->draw(dc, offset);
			break;
	}
	EndPaint(window, &paintStructure);
}

void ZLWin32ViewWidget::onMousePress(int x, int y) {
	view()->onStylusPress(x, y);
}

void ZLWin32ViewWidget::onMouseRelease(int x, int y) {
	view()->onStylusRelease(x, y);
}

void ZLWin32ViewWidget::onMouseMove(int x, int y) {
	view()->onStylusMove(x, y);
}

void ZLWin32ViewWidget::onMouseMovePressed(int x, int y) {
	ZLPaintContext &context = view()->context();
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
