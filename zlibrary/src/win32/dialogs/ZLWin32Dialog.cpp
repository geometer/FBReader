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

#include "ZLWin32Dialog.h"
#include "ZLWin32DialogContent.h"
#include "ZLWin32DialogControl.h"
#include "../application/ZLWin32ApplicationWindow.h"

ZLWin32Dialog::ZLWin32Dialog(ZLWin32ApplicationWindow *window, const std::string &name) : myWindow(window), myTitle(name) {
	myWindow->blockMouseEvents(true);
	myTab = new ZLWin32DialogContent();
	//myDialog = createWin32Dialog(name.c_str());
}

ZLWin32Dialog::~ZLWin32Dialog() {
	myWindow->blockMouseEvents(false);
	//destroyWin32Dialog(myDialog);
}

void ZLWin32Dialog::addButton(const std::string &text, bool accept) {
	myButtons.push_back(ButtonInfo(text, accept));
}

static BOOL CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
			switch (wParam) {
				case IDOK:
					EndDialog(hDialog, true);
					return true;
				case IDCANCEL:
					EndDialog(hDialog, false);
					//SendMessage(GetDlgItem(hDialog, IDCANCEL), WM_SETTEXT, 0, (LPARAM)"Test");
					//SetWindowPos(GetDlgItem(hDialog, IDCANCEL), 0, 20, 20, 70, 30, 0);
					return false;
			}
	}
	return false;
}

bool ZLWin32Dialog::run() {
	int cxChar, cyChar;
	{
		TEXTMETRIC metric;
		HDC hdc = GetDC(myWindow->mainWindow());
		GetTextMetrics(hdc, &metric);
		ReleaseDC(myWindow->mainWindow(), hdc);
		cxChar = metric.tmAveCharWidth + 1;
		cyChar = metric.tmHeight + metric.tmExternalLeading;
	}
	int dlgXUnit, dlgYUnit;
	{
		DWORD dlgUnit = GetDialogBaseUnits();
		dlgXUnit = LOWORD(dlgUnit);
		dlgYUnit = HIWORD(dlgUnit);
	}
	cxChar *= 4;
	cxChar /= dlgXUnit;
	cyChar *= 8;
	cyChar /= dlgYUnit;

	ZLWin32DialogPanel panel(DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION, 20, 20, 20 + 60 * myButtons.size(), 120, myTitle);
	for (std::vector<ButtonInfo>::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
		style = style | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
		shared_ptr<ZLWin32DialogControl> control = new ZLWin32DialogControl(style, 20 + 60 * (it - myButtons.begin()), 80, 40, cyChar * 3 / 2, it->second ? IDOK : IDCANCEL, "button", it->first);
		panel.addControl(control);
	}

	return DialogBoxIndirect(GetModuleHandle(0), panel.dialogTemplate(), myWindow->mainWindow(), DialogProc);
}
