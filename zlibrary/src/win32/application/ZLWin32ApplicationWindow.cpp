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

#include <iostream>

#include <ZLFile.h>
#include <ZLOptionEntry.h>
#include <ZLDialog.h>
#include <ZLPaintContext.h>

#include "../../abstract/util/ZLKeyUtil.h"
#include "ZLWin32ApplicationWindow.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../view/ZLWin32ViewWidget.h"
#include "../w32widgets/W32WCHARUtil.h"

const int ICON_SIZE = 24;

ZLWin32ApplicationWindow *ZLWin32ApplicationWindow::ourApplicationWindow = 0;

int ZLWin32ApplicationWindow::x(WPARAM lParam) {
	return LOWORD(lParam);
}

int ZLWin32ApplicationWindow::y(WPARAM lParam) {
	return HIWORD(lParam) - ourApplicationWindow->topOffset();
}

LRESULT CALLBACK ZLWin32ApplicationWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ourApplicationWindow->mainLoopCallback(hWnd, uMsg, wParam, lParam);
}

LRESULT ZLWin32ApplicationWindow::mainLoopCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CREATE:
			myMainWindow = hWnd;
			ZLApplicationWindow::init();
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_LBUTTONDOWN:
			if (!myBlockMouseEvents) {
				if (myWin32ViewWidget->myMouseCaptured) {
					SetCapture(myMainWindow);
				}
				myWin32ViewWidget->onMousePress(x(lParam), y(lParam));
			}
			return 0;
		case WM_LBUTTONUP:
			if (!myBlockMouseEvents) {
				myWin32ViewWidget->onMouseRelease(x(lParam), y(lParam));
			}
			if (myMainWindow == GetCapture()) {
				ReleaseCapture();
			}
			return 0;
		case WM_MOUSEMOVE:
			if (!myBlockMouseEvents) {
				if (wParam & MK_LBUTTON) {
					myWin32ViewWidget->onMouseMovePressed(x(lParam), y(lParam));
				} else {
					myWin32ViewWidget->onMouseMove(x(lParam), y(lParam));
				}
			}
			return 0;
		case WM_MOUSEWHEEL:
			if (!myBlockMouseEvents) {
				application().doActionByKey(
					((short)HIWORD(wParam) > 0) ?
						ZLApplication::MouseScrollUpKey :
						ZLApplication::MouseScrollDownKey
				);
			}
			return 0;
		case WM_KEYDOWN:
			if (wParam == 0x11) {
				myKeyboardModifierMask |= 0x2;
			} else if (wParam == 0x12) {
				myKeyboardModifierMask |= 0x4;
			} else {
				application().doActionByKey(ZLKeyUtil::keyName(wParam, wParam, myKeyboardModifierMask));
			}
			return 0;
		case WM_KEYUP:
			if (wParam == 0x11) {
				myKeyboardModifierMask &= ~0x2;
			} else if (wParam == 0x12) {
				myKeyboardModifierMask &= ~0x4;
			}
			return 0;
		case WM_SIZE:
			if (myToolbar != 0) {
				SendMessage(myToolbar, TB_AUTOSIZE, 0, 0);
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		case WM_ERASEBKGND:
			return 0;
		case WM_PAINT:
			myWin32ViewWidget->doPaint();
			return 0;
		case WM_CLOSE:
			if (!myFullScreen) {
				RECT rectangle;
				GetWindowRect(myMainWindow, &rectangle);
				myWidthOption.setValue(rectangle.right - rectangle.left + 1);
				myHeightOption.setValue(rectangle.bottom - rectangle.top + 1);
			}
			application().closeView();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_COMMAND:
			onToolbarButtonPress(LOWORD(wParam));
			return 0;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

static const std::string OPTIONS = "Options";

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 2000, 600),
	myToolbar(0),
	myBlockMouseEvents(false),
	myKeyboardModifierMask(0),
	myFullScreen(false) {

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

/*
	ZLWin32SignalUtil::connectSignal(GTK_OBJECT(myMainWindow), "delete_event", GTK_SIGNAL_FUNC(applicationQuit), this);
*/
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(wc.hInstance, TEXT("ApplicationIcon"));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = ::wchar(::createNTWCHARString(myClassName, ZLApplication::ApplicationName()));
	wc.hIconSm = wc.hIcon;

	RegisterClassEx(&wc);

	ourApplicationWindow = this;
}

void ZLWin32ApplicationWindow::init() {
	const WCHAR *aName = ::wchar(myClassName);
	myMainWindow = CreateWindow(aName, aName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, GetModuleHandle(0), 0);

	// TODO: Hmm, replace SW_SHOWDEFAULT by nCmdShow?
	ShowWindow(myMainWindow, SW_SHOWDEFAULT);
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	ourApplicationWindow = 0;
}

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	PostMessage(myToolbar, TB_CHECKBUTTON, button.actionId(), button.isPressed());
}

void ZLWin32ApplicationWindow::onToolbarButtonPress(int actionCode) {
	onButtonPress((ZLApplication::Toolbar::ButtonItem&)*myButtonByActionCode[actionCode]);
}

void ZLWin32ApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;

	int style = GetWindowLong(myMainWindow, GWL_STYLE);
	static WINDOWPLACEMENT mainPlacement;
	static WINDOWPLACEMENT toolbarPlacement;
	if (myFullScreen) {
		GetWindowPlacement(myMainWindow, &mainPlacement);
		GetWindowPlacement(myToolbar, &toolbarPlacement);
		ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
		ShowWindow(myToolbar, SW_HIDE);
		SetWindowLong(myMainWindow, GWL_STYLE, style & ~WS_CAPTION);
	} else {
		SetWindowLong(myMainWindow, GWL_STYLE, style | WS_CAPTION);
		SetWindowPlacement(myMainWindow, &mainPlacement);
		SetWindowPlacement(myToolbar, &toolbarPlacement);
		//ShowWindow(myToolbar, SW_SHOWNORMAL);
		//ShowWindow(myMainWindow, SW_SHOWNORMAL);
	}
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

HBITMAP ZLWin32ApplicationWindow::maskBitmap(HBITMAP original) {
	HDC toolbarDC = GetDC(myToolbar);
	HDC srcDC = CreateCompatibleDC(toolbarDC);
	HDC dstDC = CreateCompatibleDC(toolbarDC);
	HBITMAP result = CreateCompatibleBitmap(toolbarDC, ICON_SIZE, ICON_SIZE);
	SelectObject(srcDC, original);
	SelectObject(dstDC, result);
	static const COLORREF gray = RGB(0xC0, 0xC0, 0xC0);
	static const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
	static const COLORREF black = RGB(0x00, 0x00, 0x00);
	for (int i = 0; i < ICON_SIZE; ++i) {
		for (int j = 0; j < ICON_SIZE; ++j) {
			COLORREF pixel = GetPixel(srcDC, i, j);
			SetPixel(dstDC, i, j, (pixel == gray) ? white : black);
		}
	}
	DeleteDC(dstDC);
	DeleteDC(srcDC);
	ReleaseDC(myToolbar, toolbarDC);
	return result;
}

HBITMAP ZLWin32ApplicationWindow::grayBitmap(HBITMAP original) {
	HDC toolbarDC = GetDC(myToolbar);
	HDC srcDC = CreateCompatibleDC(toolbarDC);
	HDC dstDC = CreateCompatibleDC(toolbarDC);
	HBITMAP result = CreateCompatibleBitmap(toolbarDC, ICON_SIZE, ICON_SIZE);
	SelectObject(srcDC, original);
	SelectObject(dstDC, result);
	static const COLORREF gray = RGB(0xC0, 0xC0, 0xC0);
	static const COLORREF dark = RGB(0x80, 0x80, 0x80);
	for (int i = 0; i < ICON_SIZE; ++i) {
		for (int j = 0; j < ICON_SIZE; ++j) {
			COLORREF pixel = GetPixel(srcDC, i, j);
			SetPixel(dstDC, i, j, (pixel == gray) ? gray : dark);
		}
	}
	DeleteDC(dstDC);
	DeleteDC(srcDC);
	ReleaseDC(myToolbar, toolbarDC);
	return result;
}

HBITMAP ZLWin32ApplicationWindow::ditheredBitmap(HBITMAP original) {
	return original;
}

void ZLWin32ApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (myToolbar == 0) {
  	myToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_BORDER | TBSTYLE_FLAT, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);
		SendMessage(myToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(myToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(ICON_SIZE, ICON_SIZE));
		SendMessage(myToolbar, TB_SETINDENT, 3, 0);
		SendMessage(myToolbar, TB_SETIMAGELIST, 0, (LPARAM)ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR8 | ILC_MASK, 0, 100));
		SendMessage(myToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR8 | ILC_MASK, 0, 100));
		SendMessage(myToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR8 | ILC_MASK, 0, 100));
	}

	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	if (item->isButton()) {
		static int buttonCounter = 0;
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;

		std::string imageFileName = ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + buttonItem.iconName() + ".bmp";
		ZLFile file(imageFileName);
		HBITMAP bitmap = (HBITMAP)LoadImageA(0, file.path().c_str(), IMAGE_BITMAP, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
		if (bitmap == 0) {
			HDC dc = GetDC(myToolbar);
			bitmap = CreateCompatibleBitmap(dc, ICON_SIZE, ICON_SIZE);
			ReleaseDC(myToolbar, dc);
		}
		HBITMAP mask = maskBitmap(bitmap);
		ImageList_Add((HIMAGELIST)SendMessage(myToolbar, TB_GETIMAGELIST, 0, 0), bitmap, mask);
		ImageList_Add((HIMAGELIST)SendMessage(myToolbar, TB_GETHOTIMAGELIST, 0, 0), ditheredBitmap(bitmap), mask);
		ImageList_Add((HIMAGELIST)SendMessage(myToolbar, TB_GETDISABLEDIMAGELIST, 0, 0), grayBitmap(bitmap), mask);

		button.iBitmap = buttonCounter;
		button.fsStyle = buttonItem.isToggleButton() ? TBSTYLE_CHECK : TBSTYLE_BUTTON;
		button.idCommand = buttonItem.actionId();
		myButtonByActionCode[button.idCommand] = item;
		button.dwData = 0;
		button.iString = 0;

		++buttonCounter;
	} else {
		button.iBitmap = 6;
		button.fsStyle = TBSTYLE_SEP;
	}
	SendMessage(myToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	if (item->isButton()) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		LPARAM state = (visible ? 0 : TBSTATE_HIDDEN) | (enabled ? TBSTATE_ENABLED : 0);
		PostMessage(myToolbar, TB_SETSTATE, (WPARAM)buttonItem.actionId(), state);
		if (buttonItem.isToggleButton()) {
			setToggleButtonState(buttonItem);
		}
	}
}

void ZLWin32ApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

ZLViewWidget *ZLWin32ApplicationWindow::createViewWidget() {
	myWin32ViewWidget = new ZLWin32ViewWidget(*this);
	return myWin32ViewWidget;
}

void ZLWin32ApplicationWindow::close() {
	DestroyWindow(myMainWindow);
}

bool ZLWin32ApplicationWindow::isFullKeyboardControlSupported() const {
	return false;
}

void ZLWin32ApplicationWindow::grabAllKeys(bool) {
}

bool ZLWin32ApplicationWindow::isFingerTapEventSupported() const {
	return false;
}

bool ZLWin32ApplicationWindow::isMousePresented() const {
	return true;
}

bool ZLWin32ApplicationWindow::isKeyboardPresented() const {
	return true;
}

void ZLWin32ApplicationWindow::setCaption(const std::string &caption) {
	::setWindowText(myMainWindow, caption);
}

HWND ZLWin32ApplicationWindow::mainWindow() const {
	return myMainWindow;
}

int ZLWin32ApplicationWindow::topOffset() const {
	if ((myToolbar != 0) && (!myFullScreen)) {
		RECT toolbarRectangle;
		GetWindowRect(myToolbar, &toolbarRectangle);
		POINT p;
		p.x = toolbarRectangle.right;
		p.y = toolbarRectangle.bottom;
		ScreenToClient(myMainWindow, &p);
		return p.y;
	}
	return 0;
}

void ZLWin32ApplicationWindow::blockMouseEvents(bool block) {
	myBlockMouseEvents = block;
}
