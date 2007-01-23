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
//#include "ZLWin32Util.h"

ZLWin32Dialog::ZLWin32Dialog(const std::string &name) {
	myTab = new ZLWin32DialogContent();
	//myDialog = createWin32Dialog(name.c_str());
}

ZLWin32Dialog::~ZLWin32Dialog() {
	//destroyWin32Dialog(myDialog);
}

void ZLWin32Dialog::addButton(const std::string &text, bool accept) {
	//std::string buttonText = gtkString(text);
	//gtk_dialog_add_button(myDialog, buttonText.c_str(), accept ? GTK_RESPONSE_ACCEPT : GTK_RESPONSE_REJECT);
}

static BOOL CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool ZLWin32Dialog::run() {
	WORD *pTemplate = (PWORD)LocalAlloc(LPTR, 2000);
	WORD *p = pTemplate;

	{
		DWORD style = DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION;
		*p++ = LOWORD(style);
		*p++ = HIWORD(style);
		*p++ = 0;
		*p++ = 0;
		*p++ = 1; // cdit
		*p++ = 0; // x
		*p++ = 0; // y
		*p++ = 100; // cx
		*p++ = 100; // cy
		*p++ = 0; // no menu
		*p++ = 0; // standard window class
		p += MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, "HELLO", 5, (WCHAR*)p, 5) + 1;
		ULONG l = (ULONG)p;
		l += 3;
		l >>= 2;
		l <<= 2;
		p = (WORD*)l;
	}

	{
		DWORD style = BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
		*p++ = LOWORD(style);
		*p++ = HIWORD(style);
		*p++ = 0;
		*p++ = 0;
		*p++ = 10; // x
		*p++ = 10; // y
		*p++ = 20; // cx
		*p++ = 20; // cy
		*p++ = 1;
		p += MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, "button", 6, (WCHAR*)p, 6) + 1;
		p += MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, "button", 6, (WCHAR*)p, 6) + 1;
		*p++ = 0; // standard window class
		ULONG l = (ULONG)p;
		l += 3;
		l >>= 2;
		l <<= 2;
		p = (WORD*)l;
	}

	return DialogBoxIndirect(GetModuleHandle(0), (DLGTEMPLATE*)p, 0, DialogProc);
}
