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

static LPWORD lpwAlign(LPWORD p) {
	ULONG ul = (ULONG)p;
	ul += 3;
	ul >>= 2;
	ul <<= 2;
	return (LPWORD)ul; 
}

static int nCopyAnsiToWideChar(LPWORD lpWCStr, LPSTR lpAnsiln) {
	int cchAnsi = lstrlen(lpAnsiln);
	return MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, lpAnsiln, cchAnsi, (LPWSTR)lpWCStr, cchAnsi) + 1;
}

static void DlgTemplate(PWORD &p, DWORD style, int items, int x, int y, int cx, int cy, LPSTR txt) {
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = items;
	*p++ = x;
	*p++ = y;
	*p++ = cx;
	*p++ = cy;
	*p++ = 0;
	*p++ = 0;
	int nchar = nCopyAnsiToWideChar(p, TEXT(txt));
	p += nchar;
	p = lpwAlign((LPWORD)p);
}

class ZLWin32DialogControl {

public:
	ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text);
	int allocate(WORD *p);
	int allocationSize();

private:
	int allocateString(WORD *p, const std::string &text);

private:
	DWORD myStyle;
	int myX, myY;
	int myWidth, myHeight;
	WORD myId;
	std::string myClassName;
	std::string myText;
};

ZLWin32DialogControl::ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text) : myStyle(style), myX(x), myY(y), myWidth(width), myHeight(height), myId(id), myClassName(className), myText(text) {
}

int ZLWin32DialogControl::allocateString(WORD *p, const std::string &text) {
	ZLUnicodeUtil::Ucs2String ucs2Str;
	ZLUnicodeUtil::utf8ToUcs2(ucs2Str, text.data(), text.length());
	ucs2Str.push_back(0);
	memcpy(p, &ucs2Str.front(), 2 * ucs2Str.size());
	return ucs2Str.size();
}

int ZLWin32DialogControl::allocationSize() {
	int size = 10 + ZLUnicodeUtil::utf8Length(myClassName) + ZLUnicodeUtil::utf8Length(myText);
	return (size + 1) >> 1 << 1;
}

int ZLWin32DialogControl::allocate(WORD *p) {
	WORD *ptr = p;

	*ptr++ = LOWORD(myStyle);
	*ptr++ = HIWORD(myStyle);
	*ptr++ = 0;
	*ptr++ = 0;
	*ptr++ = myX;
	*ptr++ = myY;
	*ptr++ = myWidth;
	*ptr++ = myHeight;
	*ptr++ = myId;
	
	ptr += allocateString(ptr, myClassName);
	ptr += allocateString(ptr, myText);

	*ptr++ = 0;

	return (ptr - p + 1) >> 1 << 1;
}

bool ZLWin32Dialog::run() {
	// TODO: free memory
	WORD *pTemplate = (PWORD)LocalAlloc(LPTR, 2000);
	WORD *p = pTemplate;

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

	const int buttonNumber = myButtons.size();
	DlgTemplate(p, DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION, buttonNumber, 20, 20, 20 + 60 * buttonNumber, 120, (char*)myTitle.c_str());
	for (int i = 0; i < buttonNumber; ++i) {
		DWORD style = (i == 0) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
		style = style | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
		ZLWin32DialogControl control(style, 20 + 60 * i, 80, 40, cyChar * 3 / 2, IDOK, "button", myButtons[i].first);
		p += control.allocate(p);
	}

	int code = DialogBoxIndirect(GetModuleHandle(0), (DLGTEMPLATE*)pTemplate, myWindow->mainWindow(), DialogProc);
	LocalFree(LocalHandle(pTemplate));
	return code;
}
