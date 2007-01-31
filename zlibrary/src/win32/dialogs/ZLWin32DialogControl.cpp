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

ZLWin32DialogControl::ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text) : myStyle(style | WS_CHILD | WS_TABSTOP), myX(x), myY(y), myWidth(width), myHeight(height), myId(id), myClassName(className), myText(text) {
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
	*p++ = myWidth;
	*p++ = myHeight;
	*p++ = myId;
	
	p += ZLWin32DialogUtil::allocateString(p, myClassName);
	p += ZLWin32DialogUtil::allocateString(p, myText);

	*p++ = 0;
}

int ZLWin32DialogControl::controlNumber() const {
	return 1;
}

ZLWin32PushButton::ZLWin32PushButton(int x, int y, int width, int height, WORD id, const std::string &text) : ZLWin32DialogControl(BS_PUSHBUTTON, x, y, width, height, id, CLASS_BUTTON, text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

ZLWin32CheckBox::ZLWin32CheckBox(int x, int y, int width, int height, WORD id, const std::string &text) : ZLWin32DialogControl(BS_CHECKBOX, x, y, width, height, id, CLASS_BUTTON, text) {
}

ZLWin32LineEditor::ZLWin32LineEditor(int x, int y, int width, int height, WORD id, const std::string &text) : ZLWin32DialogControl(WS_BORDER, x, y, width, height, id, CLASS_EDIT, text) {
}
