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
#include "W32Control.h"
#include "W32WCHARUtil.h"

static const int FirstControlId = 2001;

std::map<HWND,W32DialogPanel*> W32DialogPanel::ourPanels;
const std::string W32DialogPanel::PANEL_SELECTED_EVENT = "Dialog Panel: Selected";
UINT W32DialogPanel::LAYOUT_MESSAGE = 0;

static void allocateString(WORD *&p, const std::string &text) {
	ZLUnicodeUtil::Ucs2String ucs2Str;
	::createNTWCHARString(ucs2Str, text);
	memcpy(p, ::wchar(ucs2Str), 2 * ucs2Str.size());
	p += ucs2Str.size();
}

W32DialogPanel::W32DialogPanel(HWND mainWindow, const std::string &caption) : W32ControlCollection(FirstControlId), myMainWindow(mainWindow), myCaption(caption), myAddress(0), myDialogWindow(0) {
	if (LAYOUT_MESSAGE == 0) {
		LAYOUT_MESSAGE = RegisterWindowMessageA("layout");
	}
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
	myElement->init(dialogWindow, this);
}

void W32DialogPanel::calculateSize() {
	myElement->setDimensions(myCharDimension);
	mySize = myElement->minimumSize();
}

W32Widget::Size W32DialogPanel::size() const {
	return mySize;
}

void W32DialogPanel::setSize(W32Widget::Size size) {
	mySize = size;
}

void W32DialogPanel::updateElementSize() {
	myElement->setPosition(0, 0, mySize);
}

DWORD W32DialogPanel::style() const {
	return DS_SHELLFONT | DS_CENTER | DS_MODALFRAME | WS_POPUPWINDOW | WS_CAPTION;
}

DLGTEMPLATE *W32DialogPanel::dialogTemplate() {
	if (myAddress != 0) {
		delete[] myAddress;
	}

	if ((mySize.Width == 0) && (mySize.Height == 0)) {
		calculateSize();
	}
	updateElementSize();

	const std::string fontName = "MS Shell Dlg";
	int size = 14 + ZLUnicodeUtil::utf8Length(myCaption) + ZLUnicodeUtil::utf8Length(fontName) + myElement->allocationSize();
	size += size % 2;
	myAddress = new WORD[size];

	WORD *p = myAddress;
	*p++ = LOWORD(style());
	*p++ = HIWORD(style());
	*p++ = 0;
	*p++ = 0;
	*p++ = myElement->controlNumber();
	*p++ = 0; // X
	*p++ = 0; // Y
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = 0;
	*p++ = 0;
	allocateString(p, myCaption);
	*p++ = 8; // FONT SIZE -- should be always 8?
	allocateString(p, fontName);
	if ((p - myAddress) % 2 == 1) {
		p++;
	}

	short id = FirstControlId;
	myElement->allocate(p, id);

	return (DLGTEMPLATE*)myAddress;
}

void W32DialogPanel::setElement(W32WidgetPtr element) {
	myElement = element;
}

W32Widget::Size W32DialogPanel::charDimension() const {
	return myCharDimension;
}

bool W32DialogPanel::commandCallback(WPARAM wParam) {
	W32Control *control = (*this)[LOWORD(wParam)];
	if (control != 0) {
		control->commandCallback(HIWORD(wParam));
		return true;
	}
	return false;
}

bool W32DialogPanel::notificationCallback(WPARAM wParam, LPARAM lParam) {
	W32Control *control = (*this)[LOWORD(wParam)];
	if (control != 0) {
		control->notificationCallback(lParam);
		return true;
	}
	return false;
}

void W32DialogPanel::invalidate() {
	if (myDialogWindow != 0) {
		PostMessage(myDialogWindow, LAYOUT_MESSAGE, 0, 0);
		myDoLayout = true;
	}
}

void W32DialogPanel::layout() {
	if (myDoLayout) {
		const short oldWidth = mySize.Width;
		calculateSize();
		mySize.Width = std::max(mySize.Width, oldWidth);
		// TODO: add scrollbars (?)
		updateElementSize();
		myDoLayout = false;
	}
}

const std::string &W32DialogPanel::caption() const {
	return myCaption;
}
