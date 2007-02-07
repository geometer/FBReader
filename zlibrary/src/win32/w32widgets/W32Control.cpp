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

#include <windows.h>
#include <commctrl.h>

#include "W32Element.h"
#include "../util/ZLWin32WCHARUtil.h"

static const WORD CLASS_BUTTON = 0x0080;
static const WORD CLASS_EDIT = 0x0081;
static const WORD CLASS_STATIC = 0x0082;
static const WORD CLASS_LISTBOX = 0x0083;
static const WORD CLASS_SCROLLBAR = 0x0084;
static const WORD CLASS_COMBOBOX = 0x0085;

static const WCHAR CLASSNAME_SPINNER[] = UPDOWN_CLASSW;

static HFONT controlFont = 0;

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

void W32Control::init(HWND parent, W32ControlCollection &collection) {
	myWindow = GetDlgItem(parent, collection.addControl(this));
	if (controlFont == 0) {
		LOGFONT logicalFont;
		logicalFont.lfHeight = 15;
		logicalFont.lfWidth = 0;
		logicalFont.lfEscapement = 0;
		logicalFont.lfOrientation = 0;
		logicalFont.lfWeight = FW_NORMAL;
		logicalFont.lfItalic = false;
		logicalFont.lfUnderline = false;
		logicalFont.lfStrikeOut = false;
		logicalFont.lfCharSet = DEFAULT_CHARSET;
		logicalFont.lfOutPrecision = 0;
		logicalFont.lfClipPrecision = 0;
		logicalFont.lfQuality = 0;
		logicalFont.lfPitchAndFamily = 0;
		//strcpy(logicalFont.lfFaceName, "Sans Serif");
		controlFont = CreateFontIndirect(&logicalFont);
	}
	//SendMessage(myWindow, WM_SETFONT, (WPARAM)controlFont, 0);
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

void W32Control::callback(DWORD) {
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

void W32PushButton::init(HWND parent, W32ControlCollection &collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myText);
}

W32Label::W32Label(const std::string &text) : W32Control(SS_RIGHT), myText(text), myVShift(0) {
}

void W32Label::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 1);
	mySize.Height = charDimension.Height * 3 / 2;
	myVShift = charDimension.Height / 4;
}

void W32Label::setPosition(int x, int y, Size size) {
	W32Control::setPosition(x, y + myVShift, Size(size.Width, size.Height - myVShift));
}

WORD W32Label::classId() const {
	return CLASS_STATIC;
}

void W32Label::init(HWND parent, W32ControlCollection &collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myText);
}

W32CheckBox::W32CheckBox(const std::string &text) : W32Control(BS_AUTOCHECKBOX | WS_TABSTOP), myText(text), myChecked(false) {
}

void W32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 1);
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32CheckBox::classId() const {
	return CLASS_BUTTON;
}

void W32CheckBox::init(HWND parent, W32ControlCollection &collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myText);
	SendMessage(myWindow, BM_SETCHECK, myChecked ? BST_CHECKED : BST_UNCHECKED, 0);
}

void W32CheckBox::setChecked(bool checked) {
	if (checked != myChecked) {
		myChecked = checked;
		if (myWindow != 0) {
			SendMessage(myWindow, BM_SETCHECK, myChecked ? BST_CHECKED : BST_UNCHECKED, 0);
		}
	}
}

bool W32CheckBox::isChecked() const {
	return myChecked;
}

void W32CheckBox::callback(DWORD) {
	myChecked = SendMessage(myWindow, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

W32AbstractEditor::W32AbstractEditor(DWORD style) : W32Control(style | WS_BORDER | WS_TABSTOP | ES_NOHIDESEL) {
}

WORD W32AbstractEditor::classId() const {
	return CLASS_EDIT;
}

void W32AbstractEditor::init(HWND parent, W32ControlCollection &collection) {
	W32Control::init(parent, collection);
}

W32LineEditor::W32LineEditor(const std::string &text) : W32AbstractEditor(ES_AUTOHSCROLL), myBlocked(true) {
	::createNTWCHARString(myBuffer, text);
	setEnabled(true);
}

static void getEditorString(HWND editor, ZLUnicodeUtil::Ucs2String &buffer) {
	const int length = SendMessage(editor, EM_LINELENGTH, 0, 0);
	buffer.clear();
	buffer.insert(buffer.end(), length + 1, 0);
	if (length > 0) {
		buffer[0] = length + 1;
		SendMessage(editor, EM_GETLINE, 0, (LPARAM)&buffer.front());
	}
}

static std::string getTextFromBuffer(const ZLUnicodeUtil::Ucs2String &buffer) {
	ZLUnicodeUtil::Ucs2String copy = buffer;
	copy.pop_back();
	std::string txt;
	ZLUnicodeUtil::ucs2ToUtf8(txt, copy);
	return txt;
}

void W32LineEditor::callback(DWORD hiWParam) {
	if ((hiWParam == EN_CHANGE) && !myBlocked) {
		getEditorString(myWindow, myBuffer);
	}
}

std::string W32LineEditor::text() const {
	return getTextFromBuffer(myBuffer);
}

void W32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * std::max(std::min((int)myBuffer.size() + 3, 25), 10);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32LineEditor::init(HWND parent, W32ControlCollection &collection) {
	W32AbstractEditor::init(parent, collection);
	SetWindowTextW(myWindow, ::wchar(myBuffer));
	myBlocked = false;
}

void W32LineEditor::setEnabled(bool enabled) {
	if (enabled) {
		myStyle &= ~ES_READONLY;
	} else {
		myStyle |= ES_READONLY;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
}

W32SpinBox::W32SpinBox(WORD min, WORD max, WORD initial) : W32AbstractEditor(ES_NUMBER), myMin(min), myMax(max), myValue(initial) {
}

void W32SpinBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * 9;
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
	
	static const int classNameLength = lstrlenW(CLASSNAME_SPINNER);
	memcpy(p, CLASSNAME_SPINNER, classNameLength * 2);
	p += classNameLength;
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
	if ((p - start) % 2 == 1) {
		p++;
	}
}

void W32SpinBox::init(HWND parent, W32ControlCollection &collection) {
	W32AbstractEditor::init(parent, collection);
	myControlWindow = GetDlgItem(parent, collection.addControl(this));
	SendMessage(myControlWindow, UDM_SETRANGE, 0, MAKELONG(myMax, myMin));
	SendMessage(myControlWindow, UDM_SETPOS, 0, MAKELONG(myValue, 0));
}

int W32SpinBox::allocationSize() const {
	static int const classNameLength = lstrlenW(CLASSNAME_SPINNER);
	int size = W32AbstractEditor::allocationSize() + 12 + classNameLength;
	return size + size % 2;
}

int W32SpinBox::controlNumber() const {
	return W32AbstractEditor::controlNumber() + 1;
}

void W32SpinBox::callback(DWORD hiWParam) {
	if (hiWParam == EN_CHANGE) {
		myValue = SendMessage(myControlWindow, UDM_GETPOS, 0, 0);
	}
}

unsigned short W32SpinBox::value() const {
	return myValue;
}

W32ComboBox::W32ComboBox(const std::vector<std::string> &list, int initialIndex) : W32Control(WS_VSCROLL | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_TABSTOP), myList(list), myIndex(initialIndex), myEditorWindow(0) {
	::createNTWCHARString(myBuffer, list[initialIndex]);
}

void W32ComboBox::allocate(WORD *&p, short &id) const {
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height * myList.size();
	*p++ = id++;
	*p++ = 0xFFFF;
	*p++ = classId();
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
}

void W32ComboBox::setDimensions(Size charDimension) {
	// TODO: implement
	int len = 0;
	for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
		len = std::max(ZLUnicodeUtil::utf8Length(*it), len);
	}
	len += 3;
	len = std::min(len, 28);
	mySize.Width = charDimension.Width * len;
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32ComboBox::classId() const {
	return CLASS_COMBOBOX;
}

void W32ComboBox::init(HWND parent, W32ControlCollection &collection) {
	// TODO: implement
	W32Control::init(parent, collection);
	ZLUnicodeUtil::Ucs2String buffer;
	for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
		SendMessage(myWindow, CB_ADDSTRING, 0, (LPARAM)::wchar(::createNTWCHARString(buffer, *it)));
	}
	SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
}

void W32ComboBox::setEnabled(bool enabled) {
	// TODO: implement
}

void W32ComboBox::setEditable(bool editable) {
	if (editable) {
		myStyle &= ~CBS_SIMPLE;
	} else {
		myStyle |= CBS_SIMPLE;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
}

void W32ComboBox::callback(DWORD hiWParam) {
	if (hiWParam == CBN_SELCHANGE) {
		const int index = SendMessage(myWindow, CB_GETCURSEL, 0, 0);
		const int length = SendMessage(myWindow, CB_GETLBTEXTLEN, index, 0);
		myBuffer.clear();
		myBuffer.insert(myBuffer.end(), length + 1, 0);
		if (length > 0) {
			SendMessage(myWindow, CB_GETLBTEXT, index, (LPARAM)&myBuffer.front());
		}
	} else if (hiWParam == CBN_EDITCHANGE) {
		if (myEditorWindow == 0) {
			COMBOBOXINFO info;
			GetComboBoxInfo(myWindow, &info);
			myEditorWindow = info.hwndItem;
		}
		getEditorString(myEditorWindow, myBuffer);
	}
}

std::string W32ComboBox::text() const {
	return getTextFromBuffer(myBuffer);
}
