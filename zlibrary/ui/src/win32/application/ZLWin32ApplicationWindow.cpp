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

#include <windowsx.h>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLOptionEntry.h>
#include <ZLDialog.h>
#include <ZLPaintContext.h>

#include "../../../../core/src/util/ZLKeyUtil.h"
#include "ZLWin32ApplicationWindow.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../view/ZLWin32ViewWidget.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"
#include "../time/ZLWin32Time.h"

const int ICON_SIZE = 32;

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
		case WM_TIMER:
			((ZLWin32TimeManager&)ZLTimeManager::instance()).execute(wParam);
			return 0;
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
			if (myCursor != 0) {
				SetCursor(myCursor);
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
			if (wParam == 0x10) {
				myKeyboardModifierMask |= 0x1;
			} else if (wParam == 0x11) {
				myKeyboardModifierMask |= 0x2;
			} else if (wParam == 0x12) {
				myKeyboardModifierMask |= 0x4;
			} else {
				application().doActionByKey(ZLKeyUtil::keyName(wParam, wParam, myKeyboardModifierMask));
				myKeyboardModifierMask = 0;
			}
			return 0;
		case WM_KEYUP:
			if (wParam == 0x10) {
				myKeyboardModifierMask &= ~0x1;
			} else if (wParam == 0x11) {
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
			if (myFullScreen) {
				myWindowStateOption.setValue(FULLSCREEN);
			} else if (IsMaximized(myMainWindow)) {
				myWindowStateOption.setValue(MAXIMIZED);
			} else {
				myWindowStateOption.setValue(NORMAL);
				RECT rectangle;
				GetWindowRect(myMainWindow, &rectangle);
				myXOption.setValue(rectangle.left);
				myYOption.setValue(rectangle.top);
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
		case WM_NOTIFY:
		{
			const NMHDR *notificationHeader = (const NMHDR*)lParam;
			switch (notificationHeader->code) {
				case TTN_NEEDTEXT:
				{
					TOOLTIPTEXT &tooltip = *(TOOLTIPTEXT*)lParam;
					ZLApplication::Toolbar::ItemPtr item = myButtonByActionCode[tooltip.hdr.idFrom];
					if (!item.isNull()) {
						const ZLApplication::Toolbar::ButtonItem &button =
							(const ZLApplication::Toolbar::ButtonItem&)*item;
						ZLUnicodeUtil::Ucs2String tooltipBuffer;
						::createNTWCHARString(tooltipBuffer, button.tooltip());
						size_t length = std::max(tooltipBuffer.size(), (size_t)80);
						memcpy((char*)tooltip.szText, (char*)::wchar(tooltipBuffer), 2 * length);
						tooltip.uFlags = TTF_DI_SETITEM;
					}
				}
			}
			return 0;
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

static const std::string OPTIONS = "Options";

ZLWin32ApplicationWindow::ZLWin32ApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myMainWindow(0),
	myToolbar(0),
	myBlockMouseEvents(false),
	myKeyboardModifierMask(0),
	myFullScreen(false),
	myWait(false),
	myCursor(0) {

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

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
	wc.lpszClassName = ::wchar(::createNTWCHARString(myClassName, ZLibrary::ApplicationName()));
	wc.hIconSm = wc.hIcon;

	RegisterClassEx(&wc);

	ourApplicationWindow = this;
}

void ZLWin32ApplicationWindow::init() {
	const WCHAR *aName = ::wchar(myClassName);
	CreateWindow(aName, aName, WS_OVERLAPPEDWINDOW, myXOption.value(), myYOption.value(), myWidthOption.value(), myHeightOption.value(), (HWND)0, (HMENU)0, GetModuleHandle(0), 0);

	// TODO: Hmm, replace SW_SHOWDEFAULT by nCmdShow?
	ShowWindow(myMainWindow, SW_SHOWDEFAULT);

	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			setFullscreen(true);
			break;
		case MAXIMIZED:
			ShowWindow(myMainWindow, SW_SHOWMAXIMIZED);
			break;
	}
}

ZLWin32ApplicationWindow::~ZLWin32ApplicationWindow() {
	ourApplicationWindow = 0;
}

void ZLWin32ApplicationWindow::setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button) {
	PostMessage(myToolbar, TB_CHECKBUTTON, myActionCodeById[button.actionId()], button.isPressed());
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
		ShowWindow(myToolbar, SW_SHOWNORMAL);
		ShowWindow(myMainWindow, SW_SHOWNORMAL);
		SetWindowPlacement(myMainWindow, &mainPlacement);
		SetWindowPlacement(myToolbar, &toolbarPlacement);
	}
	myWin32ViewWidget->repaint();
}

bool ZLWin32ApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLWin32ApplicationWindow::addToolbarItem(ZLApplication::Toolbar::ItemPtr item) {
	if (myToolbar == 0) {
  	//myToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);
  	myToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, myMainWindow, (HMENU)1, GetModuleHandle(0), 0);
		SendMessage(myToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(myToolbar, TB_SETBITMAPSIZE, 0, MAKELONG(ICON_SIZE, ICON_SIZE));
		SendMessage(myToolbar, TB_SETINDENT, 3, 0);
		SendMessage(myToolbar, TB_SETIMAGELIST, 0, (LPARAM)ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR32 | ILC_MASK, 0, 100));
	}

	TBBUTTON button;
	button.fsState = TBSTATE_ENABLED;
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		static int buttonCounter = 0;
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;

		std::string imageFileName = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + buttonItem.iconName() + ".ico";
		ZLFile file(imageFileName);
		HICON bitmap = (HICON)LoadImageA(0, file.path().c_str(), IMAGE_ICON, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
		/*
		if (bitmap == 0) {
			HDC dc = GetDC(myToolbar);
			bitmap = CreateCompatibleBitmap(dc, ICON_SIZE, ICON_SIZE);
			ReleaseDC(myToolbar, dc);
		}
		*/
		ImageList_AddIcon((HIMAGELIST)SendMessage(myToolbar, TB_GETIMAGELIST, 0, 0), bitmap);

		button.iBitmap = buttonCounter;
		button.fsStyle = buttonItem.isToggleButton() ? TBSTYLE_CHECK : TBSTYLE_BUTTON;
		const int actionCode = myActionCodeById.size() + 1;
		myActionCodeById[buttonItem.actionId()] = actionCode;
		button.idCommand = actionCode;
		myButtonByActionCode[button.idCommand] = item;
		button.dwData = 0;
		button.iString = 0;

		++buttonCounter;
	} else {
		static int separatorCounter = -100;
		button.idCommand = --separatorCounter;
		mySeparatorNumbers[item] = button.idCommand;
		button.iBitmap = 6;
		button.fsStyle = TBSTYLE_SEP;
	}
	SendMessage(myToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
}

void ZLWin32ApplicationWindow::setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled) {
	if (item->type() == ZLApplication::Toolbar::Item::BUTTON) {
		const ZLApplication::Toolbar::ButtonItem &buttonItem = (const ZLApplication::Toolbar::ButtonItem&)*item;
		LPARAM state = (visible ? 0 : TBSTATE_HIDDEN) | (enabled ? TBSTATE_ENABLED : 0);
		PostMessage(myToolbar, TB_SETSTATE, (WPARAM)myActionCodeById[buttonItem.actionId()], state);
		if (buttonItem.isToggleButton()) {
			setToggleButtonState(buttonItem);
		}
	} else {
		PostMessage(myToolbar, TB_SETSTATE, mySeparatorNumbers[item], visible ? 0 : TBSTATE_HIDDEN);
	}
}

ZLViewWidget *ZLWin32ApplicationWindow::createViewWidget() {
	myWin32ViewWidget = new ZLWin32ViewWidget(*this);
	return myWin32ViewWidget;
}

void ZLWin32ApplicationWindow::close() {
	DestroyWindow(myMainWindow);
}

void ZLWin32ApplicationWindow::grabAllKeys(bool) {
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

void ZLWin32ApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	static HCURSOR handCursor = LoadCursor(0, IDC_HAND);
	myCursor = hyperlink ? handCursor : 0;
}

void ZLWin32ApplicationWindow::setWait(bool wait) {
	static HCURSOR waitCursor = LoadCursor(0, IDC_WAIT);
	myCursor = wait ? waitCursor : 0;
	SetCursor(myCursor);
	myWait = wait;
}
