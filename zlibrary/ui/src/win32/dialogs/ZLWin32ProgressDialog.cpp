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

#include <ZLUnicodeUtil.h>

#include "ZLWin32ProgressDialog.h"

#include "../application/ZLWin32ApplicationWindow.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

typedef LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM);

static LRESULT CALLBACK WaitProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
		{
			ZLWin32ProgressDialog *dialog =
				(ZLWin32ProgressDialog*)GetWindowLong(hWnd, GWL_USERDATA);

			const ZLUnicodeUtil::Ucs2String &str = dialog->wcharMessage();

			RECT rect;
			SIZE size;
			GetWindowRect(hWnd, &rect);
			HDC dc = GetDC(hWnd);
			SetBkMode(dc, TRANSPARENT);

			GetTextExtentPointW(dc, ::wchar(str), str.size(), &size);

			RECT intRect;
			intRect.left = 0;
			intRect.right = rect.right - rect.left;
			intRect.top = 0;
			intRect.bottom = rect.bottom - rect.top;
			FillRect(dc, &intRect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

			TextOutW(
				dc,
				(rect.right - rect.left - size.cx) / 2,
				(rect.bottom - rect.top - size.cy) / 2,
				::wchar(str),
				str.size()
			);

			ReleaseDC(hWnd, dc);
		}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

ZLWin32ProgressDialog::ZLWin32ProgressDialog(ZLWin32ApplicationWindow *window, const ZLResourceKey &key) : ZLProgressDialog(key), myApplicationWindow(window) {
	static bool doRegister = true;
	if (doRegister) {
		doRegister = false;
		WNDCLASSEX dockc;
		dockc.cbSize = sizeof(dockc);
		dockc.style = 0;
		dockc.lpfnWndProc = (WndProc)WaitProc;
		dockc.cbClsExtra = 0;
		dockc.cbWndExtra = 0;
		dockc.hInstance = GetModuleHandle(0);
		dockc.hIcon = 0;
		dockc.hCursor = LoadCursor(0, IDC_ARROW);
		dockc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		dockc.lpszMenuName = 0;
		dockc.lpszClassName = L"WaitMessage";
		dockc.hIconSm = 0;
		RegisterClassEx(&dockc);
	}
}

ZLWin32ProgressDialog::~ZLWin32ProgressDialog() {
}

const ZLUnicodeUtil::Ucs2String &ZLWin32ProgressDialog::wcharMessage() const {
	return myWCharMessage;
}

void ZLWin32ProgressDialog::run(ZLRunnable &runnable) {
	myMessageWindow = 0;
	if (myApplicationWindow != 0) {
		myApplicationWindow->setWait(true);
		HWND mainWindow = myApplicationWindow->mainWindow();
		RECT mainRect;
		GetWindowRect(mainWindow, &mainRect);
		const int centerX = (mainRect.left + mainRect.right) / 2;
		const int centerY = (mainRect.top + mainRect.bottom) / 2;
		myMessageWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"WaitMessage", 0, WS_BORDER | WS_POPUP, centerX - 20, centerY - 5, 40, 10, mainWindow, 0, GetModuleHandle(0), 0);
		SetWindowLong(myMessageWindow, GWL_USERDATA, (LONG)this);
		setMessage(messageText());

		SIZE size;
		HDC dc = GetDC(myMessageWindow);
		GetTextExtentPointW(dc, ::wchar(myWCharMessage), myWCharMessage.size(), &size);
		ReleaseDC(myMessageWindow, dc);
		const size_t h = 4 * size.cy;
		const size_t w = size.cx + 6 * size.cy;

		SetWindowPos(myMessageWindow, HWND_TOP, centerX - w / 2, centerY - h / 2, w, h, SWP_SHOWWINDOW);

		UpdateWindow(myMessageWindow);
	}

	runnable.run();

	if (myApplicationWindow != 0) {
		ShowWindow(myMessageWindow, SW_HIDE);
		DestroyWindow(myMessageWindow);
		myApplicationWindow->setWait(false);
	}
}

void ZLWin32ProgressDialog::setMessage(const std::string &message) {
	myWCharMessage.clear();
	ZLUnicodeUtil::utf8ToUcs2(myWCharMessage, message);
	UpdateWindow(myMessageWindow);

	RECT rect;
	GetClientRect(myMessageWindow, &rect);
	InvalidateRect(myMessageWindow, &rect, false);

	MSG msg;
	GetMessage(&msg, 0, 0, 0);
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}
