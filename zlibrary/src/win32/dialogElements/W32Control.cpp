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

#include "W32Element.h"

const std::string CLASS_BUTTON = "button";
const std::string CLASS_EDIT = "edit";

W32Control::W32Control(DWORD style, WORD id, const std::string &text) : myStyle(style | WS_CHILD | WS_TABSTOP), myX(0), myY(0), myId(id), myText(text) {
}

void W32Control::setVisible(bool visible) {
	// TODO: update initialized control
	if (visible) {
		myStyle |= WS_VISIBLE;
	} else {
		myStyle &= ~WS_VISIBLE;
	}
}

int W32Control::allocationSize() const {
	int size = 12 + ZLUnicodeUtil::utf8Length(className()) + ZLUnicodeUtil::utf8Length(myText);
	return size + size % 2;
}

void W32Control::allocate(WORD *p) const {
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = myId;
	
	p += allocateString(p, className());
	p += allocateString(p, myText);

	*p++ = 0;
}

int W32Control::controlNumber() const {
	return 1;
}

W32Element::Size W32Control::minimumSize() const {
	return mySize;
}

void W32Control::setPosition(int x, int y, Size size) {
	myX = x;
	myY = y;
	mySize = size;
}

W32PushButton::W32PushButton(WORD id, const std::string &text) : W32Control(BS_PUSHBUTTON, id, text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

void W32PushButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32PushButton::className() const {
	return CLASS_BUTTON;
}

W32CheckBox::W32CheckBox(WORD id, const std::string &text) : W32Control(BS_CHECKBOX, id, text) {
}

void W32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32CheckBox::className() const {
	return CLASS_BUTTON;
}

W32LineEditor::W32LineEditor(WORD id, const std::string &text) : W32Control(WS_BORDER, id, text) {
}

void W32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32LineEditor::className() const {
	return CLASS_EDIT;
}
