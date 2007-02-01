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

#include "ZLWin32DialogElement.h"

const std::string CLASS_BUTTON = "button";
const std::string CLASS_EDIT = "edit";

ZLWin32DialogControl::ZLWin32DialogControl(DWORD style, WORD id, const std::string &className, const std::string &text) : myStyle(style | WS_CHILD | WS_TABSTOP), myX(0), myY(0), myId(id), myClassName(className), myText(text) {
}

void ZLWin32DialogControl::setVisible(bool visible) {
	// TODO: update initialized control
	if (visible) {
		myStyle |= WS_VISIBLE;
	} else {
		myStyle &= ~WS_VISIBLE;
	}
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
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = myId;
	
	p += ZLWin32DialogUtil::allocateString(p, myClassName);
	p += ZLWin32DialogUtil::allocateString(p, myText);

	*p++ = 0;
}

int ZLWin32DialogControl::controlNumber() const {
	return 1;
}

ZLWin32DialogElement::Size ZLWin32DialogControl::minimumSize() const {
	return mySize;
}

void ZLWin32DialogControl::setPosition(int x, int y, Size size) {
	myX = x;
	myY = y;
	mySize = size;
}

ZLWin32PushButton::ZLWin32PushButton(WORD id, const std::string &text) : ZLWin32DialogControl(BS_PUSHBUTTON, id, CLASS_BUTTON, text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

void ZLWin32PushButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

ZLWin32CheckBox::ZLWin32CheckBox(WORD id, const std::string &text) : ZLWin32DialogControl(BS_CHECKBOX, id, CLASS_BUTTON, text) {
}

void ZLWin32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

ZLWin32LineEditor::ZLWin32LineEditor(WORD id, const std::string &text) : ZLWin32DialogControl(WS_BORDER, id, CLASS_EDIT, text) {
}

void ZLWin32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}
