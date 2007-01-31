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

#include "ZLWin32DialogControl.h"

int ZLWin32DialogControl::allocateString(WORD *p, const std::string &text) {
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

DLGTEMPLATE *ZLWin32DialogPanel::dialogTemplate() const {
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
	p += ZLWin32DialogControl::allocateString(p, myText);
	if ((p - myAddress) % 2 == 1) {
		p++;
	}

	for (ControlList::const_iterator it = myControls.begin(); it != myControls.end(); ++it) {
		(*it)->allocate(p);
		p += (*it)->allocationSize();
	}

	return (DLGTEMPLATE*)myAddress;
}

int ZLWin32DialogPanel::allocationSize() const {
	int size = 12 + ZLUnicodeUtil::utf8Length(myText);
	size += size % 2;
	for (ControlList::const_iterator it = myControls.begin(); it != myControls.end(); ++it) {
		size += (*it)->allocationSize();
	}
	return size;
}

void ZLWin32DialogPanel::addControl(shared_ptr<ZLWin32DialogControl> control) {
	if (!control.isNull()) {
		myControls.push_back(control);
	}
}
