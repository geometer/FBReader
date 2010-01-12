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

#include "ZLWin32ApplicationWindow.h"

typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);

static LRESULT CALLBACK DockProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_NCACTIVATE:
			return DefWindowProc(hWnd, uMsg, true, lParam);
		case WM_COMMAND:
		case WM_NOTIFY:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			HWND owner = GetWindow(hWnd, GW_OWNER);
			WndProc proc = (WndProc)GetWindowLong(owner, GWL_WNDPROC);
			return proc(owner, uMsg, wParam, lParam);
		}
		case WM_CLOSE:
			ShowWindow(hWnd, SW_HIDE);
			return 0;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void ZLWin32ApplicationWindow::registerFloatingToolbarClass() {
	if (myFloatingToolbarClassRegistered) {
		return;
	}

	WNDCLASSEX dockc;
	dockc.cbSize = sizeof(dockc);
	dockc.style = 0;
	dockc.lpfnWndProc = DockProc;
	dockc.cbClsExtra = 0;
	dockc.cbWndExtra = 0;
	dockc.hInstance = GetModuleHandle(0);
	dockc.hIcon = 0;
	dockc.hCursor = LoadCursor(0, IDC_ARROW);
	dockc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	dockc.lpszMenuName = 0;
	dockc.lpszClassName = L"FloatingToolbar";
	dockc.hIconSm = 0;
	RegisterClassEx(&dockc);

	myFloatingToolbarClassRegistered = true;
}

void ZLWin32ApplicationWindow::createFloatingToolbar() {
	if (!hasFullscreenToolbar()) {
		return;
	}

	registerFloatingToolbarClass();

	myDockWindow = CreateWindowEx(WS_EX_TOOLWINDOW, L"FloatingToolbar", 0, WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN, 200, 200, 400, 164, myMainWindow, 0, GetModuleHandle(0), 0);
  myFullscreenToolbar.hwnd = CreateWindowEx(0, TOOLBARCLASSNAME, 0, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | BTNS_AUTOSIZE, 0, 0, 0, 0, myDockWindow, (HMENU)1, GetModuleHandle(0), 0);
	SendMessage(myFullscreenToolbar.hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(myFullscreenToolbar.hwnd, TB_SETBITMAPSIZE, 0, MAKELONG(IconSize, IconSize));
	SendMessage(myFullscreenToolbar.hwnd, TB_SETIMAGELIST, 0, (LPARAM)ImageList_Create(IconSize, IconSize, ILC_COLOR32 | ILC_MASK, 0, 100));

	for (std::vector<ZLToolbar::ItemPtr>::const_iterator it = myFloatingToolbarItems.begin(); it != myFloatingToolbarItems.end(); ++it) {
		addToolbarItem(*it);
	}
}

void ZLWin32ApplicationWindow::destroyFloatingToolbar() {
	if (myDockWindow != 0) {
		ShowWindow(myDockWindow, SW_HIDE);
		myFullscreenToolbar.clear();
		DestroyWindow(myDockWindow);
		myDockWindow = 0;
	}
}

void ZLWin32ApplicationWindow::updateFullscreenToolbarSize() {
	if (myDockWindow == 0) {
		return;
	}

	RECT wRect, cRect;

	int maxX = 0;
	int maxY = 0;

	const int len = SendMessage(myFullscreenToolbar.hwnd, TB_BUTTONCOUNT, 0, 0);
	int lastVisibleIndex = -1;
	for (int index = 0; index < len; ++index) {
		TBBUTTON info;
		SendMessage(myFullscreenToolbar.hwnd, TB_GETBUTTON, index, (LPARAM)&info);
		if ((info.fsState & TBSTATE_HIDDEN) == 0) {
			lastVisibleIndex = index;
		}
	}
	if (lastVisibleIndex != -1) {
		RECT rect;
		SendMessage(myFullscreenToolbar.hwnd, TB_GETITEMRECT, lastVisibleIndex, (LPARAM)&rect);
		maxX = rect.right;
		maxY = rect.bottom;
	}

	GetWindowRect(myDockWindow, &wRect);
	GetClientRect(myDockWindow, &cRect);
	const int deltaX = (wRect.right - wRect.left) - (cRect.right - cRect.left);
	const int deltaY = (wRect.bottom - wRect.top) - (cRect.bottom - cRect.top);
	SetWindowPos(myDockWindow, HWND_TOP, 0, 0, maxX + deltaX, maxY + deltaY, SWP_NOMOVE);
	SetWindowPos(myFullscreenToolbar.hwnd, HWND_TOP, 0, 0, maxX, maxY, SWP_NOMOVE);
}
