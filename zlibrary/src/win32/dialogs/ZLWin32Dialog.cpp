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
#include "ZLWin32DialogElement.h"
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
		DWORD dlgUnit = GetDialogBaseUnits();
		cxChar = (metric.tmAveCharWidth + 1) * 4 / LOWORD(dlgUnit);
		cyChar = (metric.tmHeight + metric.tmExternalLeading) * 8 / HIWORD(dlgUnit);
	}

	ZLWin32DialogPanel panel(DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION, 20, 20, 20 + 60 * myButtons.size(), 120, myTitle);
	ZLWin32DialogVBox *panelBox = new ZLWin32DialogVBox();
	panel.setElement(panelBox);

	ZLWin32DialogVBox *contentBox = new ZLWin32DialogVBox();
	ZLWin32DialogHBox *buttonBox = new ZLWin32DialogHBox();
	panelBox->addElement(contentBox);
	panelBox->addElement(buttonBox);

	ZLWin32DialogElementPtr control = new ZLWin32LineEditor(70, cyChar * 3 / 2, 10001, "My Editor");
	control->setVisible(true);
	contentBox->addElement(control);
	control = new ZLWin32CheckBox(70, cyChar * 3 / 2, 10001, "My Checkbox");
	control->setVisible(true);
	contentBox->addElement(control);
	for (std::vector<ButtonInfo>::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		control = new ZLWin32PushButton(40, cyChar * 3 / 2, it->second ? IDOK : IDCANCEL, it->first);
		control->setVisible(true);
		buttonBox->addElement(control);
	}

	return DialogBoxIndirect(GetModuleHandle(0), panel.dialogTemplate(), myWindow->mainWindow(), DialogProc);
}
