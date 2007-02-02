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

const WORD CLASS_BUTTON = 0x0080;
const WORD CLASS_EDIT = 0x0081;
const WORD CLASS_STATIC = 0x0082;
const WORD CLASS_LISTBOX = 0x0083;
const WORD CLASS_SCROLLBAR = 0x0084;
const WORD CLASS_COMBOBOX = 0x0085;

static void setText(HWND hWnd, const std::string &text) {
	ZLUnicodeUtil::Ucs2String str;
	ZLUnicodeUtil::utf8ToUcs2(str, text.data(), text.length());
	str.push_back(0);
	SetWindowTextW(hWnd, (WCHAR*)&str.front());
}

W32Control::W32Control(DWORD style) : myStyle(style | WS_CHILD), myX(1), myY(1), mySize(Size(1, 1)), myWindow(0) {
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
	return 14;
}

void W32Control::allocate(WORD *&p, short &id) const {
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = id++;
	*p++ = 0xFFFF;
	*p++ = classId();
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
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

W32PushButton::W32PushButton(const std::string &text) : W32Control(BS_PUSHBUTTON | WS_TABSTOP), myText(text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

void W32PushButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32PushButton::classId() const {
	return CLASS_BUTTON;
}

void W32PushButton::init(HWND parent, short &id) {
	W32Control::init(parent, id);
	::setText(myWindow, myText);
}

W32Label::W32Label(const std::string &text) : W32Control(SS_LEFT), myText(text) {
}

void W32Label::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32Label::classId() const {
	return CLASS_STATIC;
}

void W32Label::init(HWND parent, short &id) {
	W32Control::init(parent, id);
	::setText(myWindow, myText);
}

W32CheckBox::W32CheckBox(const std::string &text) : W32Control(BS_CHECKBOX | WS_TABSTOP), myText(text) {
}

void W32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32CheckBox::classId() const {
	return CLASS_BUTTON;
}

void W32CheckBox::init(HWND parent, short &id) {
	W32Control::init(parent, id);
	::setText(myWindow, myText);
}

W32AbstractEditor::W32AbstractEditor(DWORD style) : W32Control(style | WS_BORDER | WS_TABSTOP) {
}

WORD W32AbstractEditor::classId() const {
	return CLASS_EDIT;
}

void W32AbstractEditor::init(HWND parent, short &id) {
	W32Control::init(parent, id);
}

W32LineEditor::W32LineEditor(const std::string &text) : myText(text) {
}

void W32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32LineEditor::init(HWND parent, short &id) {
	W32AbstractEditor::init(parent, id);
	::setText(myWindow, myText);
}

W32SpinBox::W32SpinBox(WORD min, WORD max, WORD initial) : W32AbstractEditor(ES_NUMBER), myMin(min), myMax(max), myInitial(initial) {
}

void W32SpinBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * 6;
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32SpinBox::allocate(WORD *&p, short &id) const {
	W32AbstractEditor::allocate(p, id);

	WORD *start = p;

	DWORD style = UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_AUTOBUDDY | UDS_ARROWKEYS;
	if (isVisible()) {
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
	
	static const int classNameLength = lstrlenW(UPDOWN_CLASSW);
	memcpy(p, UPDOWN_CLASSW, classNameLength * 2);
	p += classNameLength;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	if ((p - start) % 2 == 1) {
		p++;
	}
}

void W32SpinBox::init(HWND parent, short &id) {
	W32AbstractEditor::init(parent, id);
	myControlWindow = GetDlgItem(parent, id++);
	SendMessage(myControlWindow, UDM_SETRANGE, 0, MAKELONG(myMax, myMin));
	SendMessage(myControlWindow, UDM_SETPOS, 0, MAKELONG(myInitial, 0));
}

int W32SpinBox::allocationSize() const {
	static int const classNameLength = lstrlenW(UPDOWN_CLASSW);
	int size = W32AbstractEditor::allocationSize() + 12 + classNameLength;
	return size + size % 2;
}

int W32SpinBox::controlNumber() const {
	return 2;
}
