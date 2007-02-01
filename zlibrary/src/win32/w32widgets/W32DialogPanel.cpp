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

#include <ZLUnicodeUtil.h>

#include "W32DialogPanel.h"

std::map<HWND,W32DialogPanel*> W32DialogPanel::ourPanels;

W32DialogPanel::W32DialogPanel(HWND mainWindow, const std::string &caption) : myCaption(caption), myAddress(0), myDialogWindow(0) {
	TEXTMETRIC metric;
	HDC hdc = GetDC(mainWindow);
	GetTextMetrics(hdc, &metric);
	ReleaseDC(mainWindow, hdc);
	DWORD dlgUnit = GetDialogBaseUnits();
	myCharDimension.Width = (metric.tmAveCharWidth + 1) * 4 / LOWORD(dlgUnit);
	myCharDimension.Height = (metric.tmHeight + metric.tmExternalLeading) * 8 / HIWORD(dlgUnit);
}

W32DialogPanel::~W32DialogPanel() {
	if (myAddress != 0) {
		delete[] myAddress;
	}
	if (myDialogWindow != 0) {
		ourPanels.erase(myDialogWindow);
	}
}

void W32DialogPanel::init(HWND dialogWindow) {
	myDialogWindow = dialogWindow;
	ourPanels[myDialogWindow] = this;	
}

void W32DialogPanel::calculateSize() {
	myElement->setDimensions(myCharDimension);
	mySize = myElement->minimumSize();
}

W32Element::Size W32DialogPanel::size() const {
	return mySize;
}

void W32DialogPanel::setSize(W32Element::Size size) {
	mySize = size;
}

DLGTEMPLATE *W32DialogPanel::dialogTemplate() {
	if (myAddress != 0) {
		delete[] myAddress;
	}

	if ((mySize.Width == 0) && (mySize.Height == 0)) {
		calculateSize();
	}
	myElement->setPosition(0, 0, mySize);

	int size = 12 + ZLUnicodeUtil::utf8Length(myCaption) + myElement->allocationSize();
	size += size % 2;
	myAddress = new WORD[size];

	WORD *p = myAddress;
	const DWORD style = DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION;
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = myElement->controlNumber();
	*p++ = 0; // X
	*p++ = 0; // Y
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = 0;
	*p++ = 0;
	W32Element::allocateString(p, myCaption);
	if ((p - myAddress) % 2 == 1) {
		p++;
	}

	short id = 2000;
	myElement->allocate(p, id);

	return (DLGTEMPLATE*)myAddress;
}

void W32DialogPanel::setElement(W32ElementPtr element) {
	myElement = element;
}

W32Element::Size W32DialogPanel::charDimension() const {
	return myCharDimension;
}

BOOL CALLBACK W32DialogPanel::StaticCallback(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_INITDIALOG) {
		((W32DialogPanel*)lParam)->init(hDialog);
		return true;
	}
	W32DialogPanel *panel = ourPanels[hDialog];
	if (panel != 0) {
		return panel->Callback(message, wParam, lParam);
	}
	return false;
}

bool W32DialogPanel::Callback(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
			switch (wParam) {
				case IDOK:
				case IDCANCEL:
					EndDialog(myDialogWindow, wParam == IDOK);
					return true;
				default:
					//std::cerr << LOWORD(wParam) << " : " << lParam << " : " << message << "\n";
					return false;
			}
	}
	return false;
}
