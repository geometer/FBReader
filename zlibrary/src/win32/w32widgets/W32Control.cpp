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

#include <windows.h>
#include <commctrl.h>

#include "W32Element.h"

const std::string CLASS_BUTTON = "button";
const std::string CLASS_EDIT = "edit";

W32Control::W32Control(DWORD style, const std::string &text) : myStyle(style | WS_CHILD | WS_TABSTOP), myX(1), myY(1), mySize(Size(1, 1)), myText(text), myWindow(0) {
}

void W32Control::setVisible(bool visible) {
	// TODO: update initialized control
	if (visible) {
		myStyle |= WS_VISIBLE;
	} else {
		myStyle &= ~WS_VISIBLE;
	}
}

bool W32Control::isVisible() const {
	return myStyle & WS_VISIBLE;
}

int W32Control::allocationSize() const {
	int size = 12 + ZLUnicodeUtil::utf8Length(className()) + ZLUnicodeUtil::utf8Length(myText);
	return size + size % 2;
}

void W32Control::allocate(WORD *&p, short &id) const {
	WORD *start = p;

	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = id++;
	
	allocateString(p, className());
	allocateString(p, myText);

	*p++ = 0;
	if ((p - start) % 2 == 1) {
		p++;
	}
}

void W32Control::init(HWND parent, short &id) {
	myWindow = GetDlgItem(parent, id++);
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

W32PushButton::W32PushButton(const std::string &text) : W32Control(BS_PUSHBUTTON, text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

void W32PushButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32PushButton::className() const {
	return CLASS_BUTTON;
}

W32CheckBox::W32CheckBox(const std::string &text) : W32Control(BS_CHECKBOX, text) {
}

void W32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32CheckBox::className() const {
	return CLASS_BUTTON;
}

W32LineEditor::W32LineEditor(const std::string &text) : W32Control(WS_BORDER, text) {
}

void W32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

const std::string &W32LineEditor::className() const {
	return CLASS_EDIT;
}

W32SpinBox::W32SpinBox(WORD min, WORD max, WORD initial) : W32LineEditor(""), myMin(min), myMax(max), myInitial(initial) {
	myStyle |= ES_NUMBER;
}

void W32SpinBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32SpinBox::allocate(WORD *&p, short &id) const {
	W32LineEditor::allocate(p, id);

	WORD *start = p;

	DWORD style = UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS;
	if (myStyle & WS_VISIBLE) {
		style |= WS_VISIBLE;
	}
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = id++;
	
	allocateString(p, UPDOWN_CLASSA);
	*p++ = 0;
	*p++ = 0;
	if ((p - start) % 2 == 1) {
		p++;
	}
}

void W32SpinBox::init(HWND parent, short &id) {
	W32Control::init(parent, id);
	myControlWindow = GetDlgItem(parent, id++);
	SendMessage(myControlWindow, UDM_SETRANGE, 0, MAKELONG(myMax, myMin));
	SendMessage(myControlWindow, UDM_SETPOS, 0, MAKELONG(myInitial, 0));
}

int W32SpinBox::allocationSize() const {
	int size = W32LineEditor::allocationSize() + 12 + ZLUnicodeUtil::utf8Length(UPDOWN_CLASSA);
	return size + size % 2;
}

int W32SpinBox::controlNumber() const {
	return 2;
}
