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

#include <windows.h>
#include <commctrl.h>

#include "W32Control.h"
#include "W32ControlCollection.h"
#include "W32WCHARUtil.h"

static const WORD CLASS_BUTTON = 0x0080;
static const WORD CLASS_EDIT = 0x0081;
static const WORD CLASS_STATIC = 0x0082;
static const WORD CLASS_LISTBOX = 0x0083;
static const WORD CLASS_SCROLLBAR = 0x0084;
static const WORD CLASS_COMBOBOX = 0x0085;

static const WCHAR CLASSNAME_SPINNER[] = UPDOWN_CLASSW;

W32Listener::W32Listener() {
}

W32Listener::~W32Listener() {
}

W32EventSender::W32EventSender() : myListener(0) {
}

void W32EventSender::setListener(W32Listener *listener) {
	myListener = listener;
}

void W32EventSender::fireEvent(const std::string &event) {
	if (myListener != 0) {
		myListener->onEvent(event, *this);
	}
}

W32Control::W32Control(DWORD style) : myStyle(style | WS_CHILD), myX(1), myY(1), mySize(Size(1, 1)), myEnabled(true), myOwner(0), myWindow(0) {
}

void W32Control::setEnabled(bool enabled) {
	if (myEnabled != enabled) {
		myEnabled = enabled;
		if (myWindow != 0) {
			EnableWindow(myWindow, myEnabled);
		}
	}
}

bool W32Control::isEnabled() const {
	return myEnabled;
}

void W32Control::setVisible(bool visible) {
	if (visible != isVisible()) {
		if (visible) {
			myStyle |= WS_VISIBLE;
		} else {
			myStyle &= ~WS_VISIBLE;
		}
		if (myWindow != 0) {
			ShowWindow(myWindow, visible ? SW_SHOW : SW_HIDE);
		}
		if (myOwner != 0) {
			myOwner->invalidate();
		}
	}
}

bool W32Control::isVisible() const {
	return (myStyle & WS_VISIBLE) == WS_VISIBLE;
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

void W32Control::init(HWND parent, W32ControlCollection *collection) {
	myOwner = collection;
	myWindow = GetDlgItem(parent, collection->addControl(this));
	if (!myEnabled) {
		EnableWindow(myWindow, false);
	}
}

int W32Control::controlNumber() const {
	return 1;
}

W32Widget::Size W32Control::minimumSize() const {
	return mySize;
}

void W32Control::setPosition(int x, int y, Size size) {
	myX = x;
	myY = y;
	mySize = size;
	if (myWindow != 0) {
		// TODO: why multiplier 2?
		SetWindowPos(myWindow, 0, x * 2, y * 2, size.Width * 2, size.Height * 2, 0);
	}
}

void W32Control::callback(DWORD) {
}

W32PushButton::W32PushButton(const std::string &text, ButtonType type) : W32Control(BS_PUSHBUTTON | WS_TABSTOP), myText(text), myType(type) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

void W32PushButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 3);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32PushButton::allocate(WORD *&p, short &id) const {
	if (myType == NORMAL_BUTTON) {
		W32Control::allocate(p, id);
	} else {
		short specialId = (myType == OK_BUTTON) ? IDOK : IDCANCEL;
		W32Control::allocate(p, specialId);
	}
}

WORD W32PushButton::classId() const {
	return CLASS_BUTTON;
}

void W32PushButton::init(HWND parent, W32ControlCollection *collection) {
	myOwner = collection;
	short id = -1;
	if (myType == OK_BUTTON) {
		id = IDOK;
	} else if (myType == CANCEL_BUTTON) {
		id = IDCANCEL;
	}
	myWindow = GetDlgItem(parent, collection->addControl(this, id));
	if (!myEnabled) {
		EnableWindow(myWindow, false);
	}
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

void W32Label::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myText);
}

const std::string W32CheckBox::STATE_CHANGED_EVENT = "CheckBox: state changed";

W32CheckBox::W32CheckBox(const std::string &text) : W32Control(BS_AUTOCHECKBOX | WS_TABSTOP), myText(text), myChecked(false) {
}

void W32CheckBox::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 1);
	mySize.Height = charDimension.Height * 3 / 2;
}

WORD W32CheckBox::classId() const {
	return CLASS_BUTTON;
}

void W32CheckBox::init(HWND parent, W32ControlCollection *collection) {
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
		fireEvent(STATE_CHANGED_EVENT);
	}
}

bool W32CheckBox::isChecked() const {
	return myChecked;
}

void W32CheckBox::callback(DWORD) {
	if (myChecked != (SendMessage(myWindow, BM_GETCHECK, 0, 0) == BST_CHECKED)) {
		myChecked = !myChecked;
		fireEvent(STATE_CHANGED_EVENT);
	}
}

W32AbstractEditor::W32AbstractEditor(DWORD style) : W32Control(style | WS_BORDER | WS_TABSTOP | ES_NOHIDESEL) {
}

WORD W32AbstractEditor::classId() const {
	return CLASS_EDIT;
}

void W32AbstractEditor::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);
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

const std::string W32LineEditor::VALUE_EDITED_EVENT = "LineEditor: value edited";

W32LineEditor::W32LineEditor(const std::string &text) : W32AbstractEditor(ES_AUTOHSCROLL), myBlocked(true) {
	::createNTWCHARString(myBuffer, text);
}

void W32LineEditor::callback(DWORD hiWParam) {
	if ((hiWParam == EN_CHANGE) && !myBlocked) {
		getEditorString(myWindow, myBuffer);
		fireEvent(VALUE_EDITED_EVENT);
	}
}

void W32LineEditor::setText(const std::string &text) {
	::createNTWCHARString(myBuffer, text);
	if (myWindow != 0) {
		SetWindowTextW(myWindow, ::wchar(myBuffer));
	}
}

std::string W32LineEditor::text() const {
	return getTextFromBuffer(myBuffer);
}

void W32LineEditor::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * std::max(std::min((int)myBuffer.size() + 3, 25), 10);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32LineEditor::init(HWND parent, W32ControlCollection *collection) {
	W32AbstractEditor::init(parent, collection);
	SetWindowTextW(myWindow, ::wchar(myBuffer));
	myBlocked = false;
}

void W32LineEditor::setEditable(bool editable) {
	if (editable) {
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

	DWORD style = UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS;
	if (isVisible()) {
		style |= WS_VISIBLE;
	}
	*p++ = LOWORD(style);
	*p++ = HIWORD(style);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX + mySize.Width;
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

void W32SpinBox::setPosition(int x, int y, Size size) {
	W32Control::setPosition(x, y, size);
	if (myControlWindow != 0) {
		SetWindowPos(myControlWindow, 0, (x + size.Width) * 2, y * 2, size.Width * 2, size.Height * 2, 0);
		SendMessage(myControlWindow, UDM_SETBUDDY, (WPARAM)myWindow, 0);
		if (isVisible()) {
			ShowWindow(myControlWindow, SW_SHOW);
		}
	}
}

void W32SpinBox::init(HWND parent, W32ControlCollection *collection) {
	W32AbstractEditor::init(parent, collection);
	myControlWindow = GetDlgItem(parent, collection->addControl(this));
	SendMessage(myControlWindow, UDM_SETBUDDY, (WPARAM)myWindow, 0);
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

void W32SpinBox::setVisible(bool visible) {
	W32Control::setVisible(visible);
	if (myControlWindow != 0) {
		ShowWindow(myControlWindow, visible ? SW_SHOW : SW_HIDE);
	}
}

void W32SpinBox::callback(DWORD hiWParam) {
	if (hiWParam == EN_CHANGE) {
		myValue = SendMessage(myControlWindow, UDM_GETPOS, 0, 0);
	}
}

unsigned short W32SpinBox::value() const {
	return myValue;
}

const std::string W32ComboBox::SELECTION_CHANGED_EVENT = "ComboBox: selection changed";
const std::string W32ComboBox::VALUE_EDITED_EVENT = "ComboBox: value edited";

W32ComboBox::W32ComboBox(const std::vector<std::string> &list, int initialIndex) : W32Control(CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP), myList(list), myIndex(initialIndex), myEditorWindow(0) {
	::createNTWCHARString(myBuffer, list[initialIndex]);
}

void W32ComboBox::setDimensions(Size charDimension) {
	int len = 10;
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

void W32ComboBox::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);
	ZLUnicodeUtil::Ucs2String buffer;
	for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
		SendMessage(myWindow, CB_ADDSTRING, 0, (LPARAM)::wchar(::createNTWCHARString(buffer, *it)));
	}
	SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
	SendMessage(myWindow, CB_SETMINVISIBLE, std::min((int)myList.size(), 7), 0);
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

HWND W32ComboBox::editorWindow() {
	if (myEditorWindow == 0) {
		COMBOBOXINFO info;
		GetComboBoxInfo(myWindow, &info);
		myEditorWindow = info.hwndItem;
	}
	return myEditorWindow;
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
		fireEvent(SELECTION_CHANGED_EVENT);
	} else if (hiWParam == CBN_EDITCHANGE) {
		getEditorString(editorWindow(), myBuffer);
		fireEvent(VALUE_EDITED_EVENT);
	}
}

std::string W32ComboBox::text() const {
	return getTextFromBuffer(myBuffer);
}

void W32ComboBox::setList(const std::vector<std::string> &list) {
	myList = list;
	if (myWindow != 0) {
		SendMessage(myWindow, CB_RESETCONTENT, 0, 0);
		ZLUnicodeUtil::Ucs2String buffer;
		for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
			SendMessage(myWindow, CB_ADDSTRING, 0, (LPARAM)::wchar(::createNTWCHARString(buffer, *it)));
		}
		SendMessage(myWindow, CB_SETMINVISIBLE, std::min((int)myList.size(), 7), 0);
	}
}

void W32ComboBox::setSelection(int index) {
	myIndex = index;
	if (myWindow != 0) {
		SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
	}
}

W32RadioButton::W32RadioButton(W32RadioButtonGroup &group, const std::string &text) : W32Control(BS_RADIOBUTTON | WS_TABSTOP), myGroup(group), myText(text) {
}

WORD W32RadioButton::classId() const {
	return CLASS_BUTTON;
}

void W32RadioButton::setDimensions(Size charDimension) {
	mySize.Width = charDimension.Width * (ZLUnicodeUtil::utf8Length(myText) + 2);
	mySize.Height = charDimension.Height * 3 / 2;
}

void W32RadioButton::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myText);
}

void W32RadioButton::callback(DWORD hiWParam) {
	if (hiWParam == BN_CLICKED) {
		if (SendMessage(myWindow, BM_GETCHECK, 0, 0) != BST_CHECKED) {
			myGroup.setChecked(*this);
		}
	}
}

void W32RadioButton::setChecked(bool checked) {
	if (myWindow != 0) {
		SendMessage(myWindow, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
	}
}

W32RadioButtonGroup::W32RadioButtonGroup(const std::string &caption, const std::vector<std::string> &buttonTexts) : W32Control(BS_GROUPBOX), myCaption(caption), myCheckedIndex(-1) {
	myButtons.reserve(buttonTexts.size());
	for (std::vector<std::string>::const_iterator it = buttonTexts.begin(); it != buttonTexts.end(); ++it) {
		myButtons.push_back(new W32RadioButton(*this, *it));
	}
}

WORD W32RadioButtonGroup::classId() const {
	return CLASS_BUTTON;
}

void W32RadioButtonGroup::allocate(WORD *&p, short &id) const {
	W32Control::allocate(p, id);
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->allocate(p, id);
	}
}

int W32RadioButtonGroup::allocationSize() const {
	int size = W32Control::allocationSize();
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		size += (*it)->allocationSize();
	}
	return size;
}

void W32RadioButtonGroup::setVisible(bool visible) {
	W32Control::setVisible(visible);
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->setVisible(visible);
	}
}

int W32RadioButtonGroup::controlNumber() const {
	int counter = W32Control::controlNumber();
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		counter += (*it)->controlNumber();
	}
	return counter;
}

void W32RadioButtonGroup::setPosition(int x, int y, Size size) {
	W32Control::setPosition(x, y, size);
	const short deltaY = size.Height / (myButtons.size() + 1);
	size.Width -= 2 * myLeftMargin;
	size.Height = deltaY;
	for (W32WidgetList::iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		y += deltaY;
		(*it)->setPosition(x + myLeftMargin, y, size)	;
	}
}

void W32RadioButtonGroup::setDimensions(Size charDimension) {
	mySize.Width = 0;
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->setDimensions(charDimension);
		mySize.Width = std::max(mySize.Width, (*it)->minimumSize().Width);
	}
	mySize.Width += 4 * charDimension.Width;
	mySize.Height = (3 * myButtons.size() + 4) * charDimension.Height / 2;
	myLeftMargin = 2 * charDimension.Width;
}

void W32RadioButtonGroup::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);
	::setWindowText(myWindow, myCaption);
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->init(parent, collection);
		((W32RadioButton&)**it).setChecked(it == myButtons.begin() + myCheckedIndex);
	}
}

void W32RadioButtonGroup::setChecked(W32RadioButton &button) {
	myCheckedIndex = -1;
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		W32RadioButton &rb = (W32RadioButton&)**it;
		if (&rb == &button) {
			rb.setChecked(true);
			myCheckedIndex = it - myButtons.begin();
		} else {
			rb.setChecked(false);
		}
	}
}

void W32RadioButtonGroup::setChecked(int index) {
	myCheckedIndex = index;
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		W32RadioButton &rb = (W32RadioButton&)**it;
		rb.setChecked(it == myButtons.begin() + index);
	}
}

int W32RadioButtonGroup::checkedIndex() const {
	return myCheckedIndex;
}
