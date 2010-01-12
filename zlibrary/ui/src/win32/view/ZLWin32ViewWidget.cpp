/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include "ZLWin32ViewWidget.h"
#include "ZLWin32PaintContext.h"

static ZLWin32ViewWidget *viewWidgetInstance = 0;

LRESULT CALLBACK ZLWin32ViewWidget::ViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return viewWidgetInstance->Callback(hWnd, uMsg, wParam, lParam);
}

LRESULT ZLWin32ViewWidget::Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WndProc orig = myOriginalCallback;

	switch (uMsg) {
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return 0;
		case WM_LBUTTONDOWN:
			if (!myWindow.mouseEventsAreBlocked()) {
				myWindow.resetFocus();
				if (myMouseCaptured) {
					SetCapture(myHandle);
				}
				onMousePress(LOWORD(lParam), HIWORD(lParam));
			}
			return 0;
		case WM_LBUTTONUP:
			if (!myWindow.mouseEventsAreBlocked()) {
				onMouseRelease(LOWORD(lParam), HIWORD(lParam));
			}
			if (myHandle == GetCapture()) {
				ReleaseCapture();
			}
			return 0;
		case WM_MOUSEMOVE:
			if (!myWindow.mouseEventsAreBlocked()) {
				if (wParam & MK_LBUTTON) {
					onMouseMovePressed(LOWORD(lParam), HIWORD(lParam));
				} else {
					onMouseMove(LOWORD(lParam), HIWORD(lParam));
				}
			}
			myWindow.updateCursor();
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_PAINT:
			doPaint();
			return 0;
		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			const int dir = (uMsg == WM_VSCROLL) ? SB_VERT : SB_HORZ;
			const size_t extra = 
				(uMsg == WM_VSCROLL) ? myVScrollBarExtra : myHScrollBarExtra;
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_ALL;
			GetScrollInfo(myHandle, dir, &info);
			ZLView::Direction direction =
				(uMsg == WM_VSCROLL) ? ZLView::VERTICAL : ZLView::HORIZONTAL;
			switch (LOWORD(wParam)) {
				case SB_TOP:
					onScrollbarPageStep(direction, -1);
					break;
				case SB_BOTTOM:
					onScrollbarPageStep(direction, 1);
					break;
				case SB_LINEUP:
					onScrollbarStep(direction, -1);
					break;
				case SB_LINEDOWN:
					onScrollbarStep(direction, 1);
					break;
				case SB_PAGEUP:
					onScrollbarPageStep(direction, -1);
					break;
				case SB_PAGEDOWN:
					onScrollbarPageStep(direction, 1);
					break;
				case SB_THUMBTRACK:
					info.nPos = info.nTrackPos;
					info.fMask = SIF_POS;
					SetScrollInfo(myHandle, dir, &info, true);
					info.fMask = SIF_ALL;
					GetScrollInfo(myHandle, dir, &info);
					onScrollbarMoved(
						direction,
						info.nMax - info.nMin - extra,
						info.nPos,
						info.nPos + info.nPage - extra
					);
					break;
			}
			SendMessage(hWnd, WM_PAINT, 0, 0);
			return orig(hWnd, uMsg, wParam, lParam);
		}
		case WM_CAPTURECHANGED:
			repaint();
			return orig(hWnd, uMsg, wParam, lParam);
		default:
			return orig(hWnd, uMsg, wParam, lParam);
	}
}

ZLWin32ViewWidget::ZLWin32ViewWidget(ZLWin32ApplicationWindow &window) : ZLViewWidget((ZLView::Angle)window.application().AngleStateOption.value()), myWindow(window), myMouseCaptured(false), myRotator(0), myHScrollBarIsEnabled(false), myVScrollBarIsEnabled(false) {
	viewWidgetInstance = this;
	myHandle = CreateWindow(WC_EDIT, 0, (WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL) & ~WS_BORDER, 0, 0, 0, 0, myWindow.mainWindow(), 0, GetModuleHandle(0), 0);
	myOriginalCallback = (WndProc)SetWindowLong(myHandle, GWL_WNDPROC, (LONG)ViewWndProc);
	ShowScrollBar(myHandle, SB_BOTH, false);
}

void ZLWin32ViewWidget::trackStylus(bool track) {
	myMouseCaptured = track;
}

void ZLWin32ViewWidget::repaint()	{
	RECT rectangle;
	GetClientRect(myHandle, &rectangle);
	InvalidateRect(myHandle, &rectangle, false);
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

void ZLWin32ViewWidget::Rotator::draw(HDC dc) {
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
	SetDIBitsToDevice(dc, 0, 0, myWidth, myHeight, 0, 0, 0, myHeight, myBuffer1, &bi, DIB_RGB_COLORS);
}

void ZLWin32ViewWidget::doPaint()	{
	ZLWin32PaintContext &win32Context = (ZLWin32PaintContext&)view()->context();

	RECT rectangle;
	GetClientRect(myHandle, &rectangle);
	const int width = rectangle.right - rectangle.left;
	const int height = rectangle.bottom - rectangle.top;

	switch (rotation()) {
		case ZLView::DEGREES0:
		case ZLView::DEGREES180:
			win32Context.updateInfo(myHandle, width, height);
			break;
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			win32Context.updateInfo(myHandle, height, width);
			break;
	}
	view()->paint();

	PAINTSTRUCT paintStructure;
	HDC dc = BeginPaint(myHandle, &paintStructure);
	switch (rotation()) {
		case ZLView::DEGREES0:
			if (myRotator != 0) {
				delete myRotator;
				myRotator = 0;
			}
			SetMapMode(dc, MM_TEXT);
			BitBlt(dc, 0, 0, width, height, win32Context.displayContext(), 0, 0, SRCCOPY);
			break;
		case ZLView::DEGREES180:
		{
			if (myRotator != 0) {
				delete myRotator;
				myRotator = 0;
			}
			SetMapMode(dc, MM_ANISOTROPIC);
			SIZE size;
			GetViewportExtEx(dc, &size);
			SetViewportExtEx(dc, -size.cx, -size.cy, 0);
			SetViewportOrgEx(dc, width - 1, height - 1, 0);
			BitBlt(dc, 0, 0, width + 1, height + 1, win32Context.displayContext(), 0, -1, SRCCOPY);
			break;
		}
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			if (myRotator == 0) {
				myRotator = new Rotator(width, height);
			} else {
				myRotator->setSize(width, height);
			}

			myRotator->setRotation(rotation() == ZLView::DEGREES90);
			myRotator->retrieve(win32Context.displayContext(), win32Context.buffer());
			myRotator->rotate();
			myRotator->draw(dc);
			break;
	}
	EndPaint(myHandle, &paintStructure);
}

void ZLWin32ViewWidget::rotateXY(int &x, int &y) const {
	ZLPaintContext &context = view()->context();
	const int maxX = context.width() - 1;
	const int maxY = context.height() - 1;
	switch (rotation()) {
		case ZLView::DEGREES0:
			break;
		case ZLView::DEGREES90:
		{
			int tmp = maxX - y;
			y = x;
			x = tmp;
			break;
		}
		case ZLView::DEGREES180:
			x = maxX - x;
			y = maxY - y;
			break;
		case ZLView::DEGREES270:
		{
			int tmp = y;
			y = maxY - x;
			x = tmp;
			break;
		}
	}
}

void ZLWin32ViewWidget::onMousePress(int x, int y) {
	rotateXY(x, y);
	view()->onStylusPress(x, y);
}

void ZLWin32ViewWidget::onMouseRelease(int x, int y) {
	rotateXY(x, y);
	view()->onStylusRelease(x, y);
}

void ZLWin32ViewWidget::onMouseMove(int x, int y) {
	rotateXY(x, y);
	view()->onStylusMove(x, y);
}

void ZLWin32ViewWidget::onMouseMovePressed(int x, int y) {
	rotateXY(x, y);
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

void ZLWin32ViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
	if (direction == ZLView::VERTICAL) {
		if (enabled == myVScrollBarIsEnabled) {
			return;
		}
		myVScrollBarIsEnabled = enabled;
	} else {
		if (enabled == myHScrollBarIsEnabled) {
			return;
		}
		myHScrollBarIsEnabled = enabled;
	}

	ShowScrollBar(myHandle, (direction == ZLView::VERTICAL) ? SB_VERT : SB_HORZ, enabled);

	doPaint();
}

void ZLWin32ViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
	/*
	int style = GetWindowLong(xxx, GWL_STYLE);
	if (direction == ZLView::VERTICAL) {
		xxx 
	SetWindowLong(myMainWindow, GWL_STYLE, style & ~WS_CAPTION);
	*/
}

void ZLWin32ViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
	if (direction == ZLView::VERTICAL) {
		if (!myVScrollBarIsEnabled) {
			return;
		}
	} else {
		if (!myHScrollBarIsEnabled) {
			return;
		}
	}

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	info.nMin = 0;
	info.nMax = full;
	info.nPage = to - from;
	info.nPos = from;
	size_t extra = 0;
	if (info.nPage < full / 20) {
		extra = full / 20 - info.nPage;
		info.nPage = full / 20;
	}
	info.nMax += extra;
	if (direction == ZLView::VERTICAL) {
		myVScrollBarExtra = extra;
	} else {
		myHScrollBarExtra = extra;
	}
	SetScrollInfo(myHandle, (direction == ZLView::VERTICAL) ? SB_VERT : SB_HORZ, &info, true);
}

HWND ZLWin32ViewWidget::handle() const {
	return myHandle;
}
