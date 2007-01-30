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

#include <ZLUnicodeUtil.h>

#include "ZLWin32Dialog.h"
#include "ZLWin32DialogContent.h"
#include "../application/ZLWin32ApplicationWindow.h"
//#include "ZLWin32Util.h"

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
	//std::string buttonText = gtkString(text);
	//gtk_dialog_add_button(myDialog, buttonText.c_str(), accept ? GTK_RESPONSE_ACCEPT : GTK_RESPONSE_REJECT);
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
					return true;
			}
	}
	return false;
}

ZLWin32DialogElement::ZLWin32DialogElement() {
}

int ZLWin32DialogElement::allocateString(WORD *p, const std::string &text) {
	ZLUnicodeUtil::Ucs2String ucs2Str;
	ZLUnicodeUtil::utf8ToUcs2(ucs2Str, text.data(), text.length());
	ucs2Str.push_back(0);
	memcpy(p, &ucs2Str.front(), 2 * ucs2Str.size());
	return ucs2Str.size();
}

ZLWin32DialogControl::ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text) : myStyle(style), myX(x), myY(y), myWidth(width), myHeight(height), myId(id), myClassName(className), myText(text) {
}

int ZLWin32DialogControl::allocationSize() const {
	int size = 12 + ZLUnicodeUtil::utf8Length(myClassName) + ZLUnicodeUtil::utf8Length(myText);
	return size + size % 2;
}

void ZLWin32DialogControl::allocate(WORD *p) const {
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = myWidth;
	*p++ = myHeight;
	*p++ = myId;
	
	p += allocateString(p, myClassName);
	p += allocateString(p, myText);

	*p++ = 0;
}

ZLWin32DialogPanel::ZLWin32DialogPanel(DWORD style, int x, int y, int width, int height, const std::string &text) : myStyle(style), myX(x), myY(y), myWidth(width), myHeight(height), myText(text), myAddress(0) {
}

ZLWin32DialogPanel::~ZLWin32DialogPanel() {
	if (myAddress != 0) {
		delete[] myAddress;
	}
}

DLGTEMPLATE *ZLWin32DialogPanel::allocate() const {
	if (myAddress != 0) {
		delete[] myAddress;
	}
	myAddress = new WORD[allocationSize()];

	WORD *p = myAddress;
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myControls.size();
	*p++ = myX;
	*p++ = myY;
	*p++ = myWidth;
	*p++ = myHeight;
	*p++ = 0;
	*p++ = 0;
	p += allocateString(p, myText);
	if ((p - myAddress) % 2 == 1) {
		p++;
	}

	for (std::vector<ZLWin32DialogControl>::const_iterator it = myControls.begin(); it != myControls.end(); ++it) {
		it->allocate(p);
		p += it->allocationSize();
	}

	return (DLGTEMPLATE*)myAddress;
}

int ZLWin32DialogPanel::allocationSize() const {
	int size = 12 + ZLUnicodeUtil::utf8Length(myText);
	size += size % 2;
	for (std::vector<ZLWin32DialogControl>::const_iterator it = myControls.begin(); it != myControls.end(); ++it) {
		size += it->allocationSize();
	}
	return size;
}

void ZLWin32DialogPanel::addControl(ZLWin32DialogControl &control) {
	myControls.push_back(control);
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
		ZLWin32DialogControl control(style, 20 + 60 * (it - myButtons.begin()), 80, 40, cyChar * 3 / 2, IDOK, "button", it->first);
		panel.addControl(control);
	}

	return DialogBoxIndirect(GetModuleHandle(0), panel.allocate(), myWindow->mainWindow(), DialogProc);
}
