/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "../../../../core/src/util/ZLKeyUtil.h"

#include "W32Control.h"
#include "W32ControlCollection.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

static const WORD CLASS_BUTTON = 0x0080;
static const WORD CLASS_EDIT = 0x0081;
static const WORD CLASS_STATIC = 0x0082;
static const WORD CLASS_LISTBOX = 0x0083;
static const WORD CLASS_SCROLLBAR = 0x0084;
static const WORD CLASS_COMBOBOX = 0x0085;

static const WCHAR CLASSNAME_SPINNER[] = UPDOWN_CLASSW;

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

void W32Control::init(HWND parent, W32ControlCollection *collection) {
	myOwner = collection;
	myWindow = GetDlgItem(parent, collection->addControl(this));
	if (!myEnabled) {
		EnableWindow(myWindow, false);
	}
}

W32StandardControl::W32StandardControl(DWORD style) : W32Control(style) {
}

int W32StandardControl::controlNumber() const {
	return 1;
}

int W32StandardControl::allocationSize() const {
	return 14;
}

void W32StandardControl::allocate(WORD *&p, short &id) const {
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	/*
	if ((mySize.Width == 0) || (mySize.Height == 0)) {
		mySize = minimumSize();
	}
	*/
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = id++;
	*p++ = 0xFFFF;
	*p++ = classId();
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;
}

void W32Control::setPosition(int x, int y, Size size) {
	bool doMove = (myX != x) || (myY != y);
	bool doResize = mySize != size;
	myX = x;
	myY = y;
	mySize = size;
	if ((doResize || doMove) && (myWindow != 0)) {
		RECT r;
		r.left = x;
		r.top = y;
		r.right = x + size.Width;
		r.bottom = y + size.Height;
		MapDialogRect(GetParent(myWindow), &r);
		DWORD flags = SWP_NOZORDER | SWP_NOOWNERZORDER;
		if (!doResize) {
			flags |= SWP_NOSIZE;
		}
		if (!doMove) {
			flags |= SWP_NOMOVE;
		}
		SetWindowPos(myWindow, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, flags);
	}
}

void W32Control::commandCallback(DWORD) {
}

void W32Control::notificationCallback(LPARAM) {
}

void W32Control::drawItemCallback(DRAWITEMSTRUCT&) {
}

const std::string W32PushButton::RELEASED_EVENT = "PushButton: released";

W32PushButton::W32PushButton(const std::string &text, ButtonType type) : W32StandardControl(BS_PUSHBUTTON | WS_TABSTOP), myText(text), myType(type) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}

W32Widget::Size W32PushButton::minimumSize() const {
	return Size(4 * (ZLUnicodeUtil::utf8Length(myText) + 3), 15);
}

void W32PushButton::allocate(WORD *&p, short &id) const {
	if (myType == NORMAL_BUTTON) {
		W32StandardControl::allocate(p, id);
	} else {
		short specialId = (myType == OK_BUTTON) ? IDOK : IDCANCEL;
		W32StandardControl::allocate(p, specialId);
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

void W32PushButton::commandCallback(DWORD) {
	fireEvent(RELEASED_EVENT);
}

W32Label::W32Label(const std::string &text, Alignment alignment) : W32StandardControl(alignment), myText(text) {
	myStringCounter = 1;
	myMaxLength = 0;
	int index = 0;
	int newIndex;
	while ((newIndex = myText.find('\n', index)) != -1) {
		++myStringCounter;
		myMaxLength = std::max(
			myMaxLength,
			(unsigned short)ZLUnicodeUtil::utf8Length(myText.data() + index, newIndex - index)
		);
		index = newIndex + 1;
	}
	myMaxLength = std::max(
		myMaxLength,
		(unsigned short)ZLUnicodeUtil::utf8Length(myText.data() + index, myText.length() - index)
	);
}

W32Widget::Size W32Label::minimumSize() const {
	return Size(4 * myMaxLength, 12 * myStringCounter);
}

void W32Label::setPosition(int x, int y, Size size) {
	::setWindowText(myWindow, "");
	W32StandardControl::setPosition(x, y + 2, Size(size.Width, size.Height - 2));
	::setWindowText(myWindow, myText);
}

WORD W32Label::classId() const {
	return CLASS_STATIC;
}

void W32Label::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	::setWindowText(myWindow, myText);
}

W32StandardIcon::W32StandardIcon(IconId iconId) : W32StandardControl(SS_CENTER | SS_ICON), myIconId(iconId) {
}

W32Widget::Size W32StandardIcon::minimumSize() const {
	// TODO: why multiplier 2?
	return Size(SM_CXICON * 2, SM_CYICON * 2);
}

WORD W32StandardIcon::classId() const {
	return CLASS_STATIC;
}

void W32StandardIcon::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	WCHAR *icon;
	switch (myIconId) {
		case ID_INFORMATION:
			icon = IDI_INFORMATION;
			break;
		case ID_QUESTION:
			icon = IDI_QUESTION;
			break;
		case ID_ERROR:
			icon = IDI_ERROR;
			break;
		default:
			icon = IDI_APPLICATION;
			break;
	}
	SendMessage(myWindow, STM_SETICON, (WPARAM)LoadIcon(0, icon), 0);
}

const std::string W32CheckBox::STATE_CHANGED_EVENT = "CheckBox: state changed";

W32CheckBox::W32CheckBox(const std::string &text) : W32StandardControl(BS_AUTOCHECKBOX | WS_TABSTOP), myText(text), myChecked(false) {
}

W32Widget::Size W32CheckBox::minimumSize() const {
	return Size(4 * (ZLUnicodeUtil::utf8Length(myText) + 1), 12);
}

WORD W32CheckBox::classId() const {
	return CLASS_BUTTON;
}

void W32CheckBox::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	::setWindowText(myWindow, myText);
	SendMessage(myWindow, BM_SETCHECK, myChecked ? BST_CHECKED : BST_UNCHECKED, 0);
}

void W32CheckBox::setEditable(bool editable) {
	if (editable) {
		myStyle &= ~WS_DISABLED;
	} else {
		myStyle |= WS_DISABLED;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
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

void W32CheckBox::commandCallback(DWORD) {
	if (myChecked != (SendMessage(myWindow, BM_GETCHECK, 0, 0) == BST_CHECKED)) {
		myChecked = !myChecked;
		fireEvent(STATE_CHANGED_EVENT);
	}
}

const std::string W32TristateBox::STATE_CHANGED_EVENT = "TristateBox: state changed";

int W32TristateBox::buttonState(ZLBoolean3 state) {
	switch (state) {
		case B3_TRUE:
			return BST_CHECKED;
		case B3_FALSE:
			return BST_UNCHECKED;
		default:
			return BST_INDETERMINATE;
	}
}

ZLBoolean3 W32TristateBox::b3State(int state) {
	switch (state) {
		case BST_CHECKED:
			return B3_TRUE;
		case BST_UNCHECKED:
			return B3_FALSE;
		default:
			return B3_UNDEFINED;
	}
}

W32TristateBox::W32TristateBox(const std::string &text) : W32StandardControl(BS_AUTO3STATE | WS_TABSTOP), myText(text), myState(B3_UNDEFINED) {
}

W32Widget::Size W32TristateBox::minimumSize() const {
	return Size(4 * (ZLUnicodeUtil::utf8Length(myText) + 1), 12);
}

WORD W32TristateBox::classId() const {
	return CLASS_BUTTON;
}

void W32TristateBox::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	::setWindowText(myWindow, myText);
	SendMessage(myWindow, BM_SETCHECK, buttonState(myState), 0);
}

void W32TristateBox::setEditable(bool editable) {
	if (editable) {
		myStyle &= ~WS_DISABLED;
	} else {
		myStyle |= WS_DISABLED;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
}

void W32TristateBox::setState(ZLBoolean3 state) {
	if (state != myState) {
		myState = state;
		if (myWindow != 0) {
			SendMessage(myWindow, BM_SETCHECK, buttonState(myState), 0);
		}
		fireEvent(STATE_CHANGED_EVENT);
	}
}

ZLBoolean3 W32TristateBox::state() const {
	return myState;
}

void W32TristateBox::commandCallback(DWORD) {
	ZLBoolean3 state = b3State(SendMessage(myWindow, BM_GETCHECK, 0, 0));
	if (state != myState) {
		myState = state;
		fireEvent(STATE_CHANGED_EVENT);
	}
}

W32AbstractEditor::W32AbstractEditor(DWORD style) : W32StandardControl(style | WS_BORDER | WS_TABSTOP | ES_NOHIDESEL) {
}

WORD W32AbstractEditor::classId() const {
	return CLASS_EDIT;
}

void W32AbstractEditor::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
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

void W32LineEditor::commandCallback(DWORD hiWParam) {
	if ((hiWParam == EN_CHANGE) && !myBlocked) {
		const int length = SendMessage(myWindow, EM_LINELENGTH, 0, 0);
		myBuffer.clear();
		myBuffer.insert(myBuffer.end(), length + 1, 0);
		if (length > 0) {
			myBuffer[0] = length + 1;
			SendMessage(myWindow, EM_GETLINE, 0, (LPARAM)&myBuffer.front());
		}
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

W32Widget::Size W32LineEditor::minimumSize() const {
	return Size(4 * std::max(std::min((int)myBuffer.size() + 3, 25), 10), 12);
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

std::map<HWND,W32KeyNameEditor*> W32KeyNameEditor::ourEditors;
const std::string W32KeyNameEditor::TEXT_CHANGED_EVENT = "KeyNameEditor: text changed";

W32KeyNameEditor::W32KeyNameEditor() : W32AbstractEditor(ES_AUTOHSCROLL), myKeyboardModifierMask(0) {
	::createNTWCHARString(myBuffer, "");
}

W32KeyNameEditor::~W32KeyNameEditor() {
	if (myWindow != 0) {
		ourEditors.erase(myWindow);
	}
}

void W32KeyNameEditor::clear() {
	setText(std::string());
}

void W32KeyNameEditor::setText(const std::string &text) {
	if (text != this->text()) {
		::createNTWCHARString(myBuffer, text);
		if (myWindow != 0) {
			SetWindowTextW(myWindow, ::wchar(myBuffer));
		}
		fireEvent(TEXT_CHANGED_EVENT);
	}
}

std::string W32KeyNameEditor::text() const {
	return getTextFromBuffer(myBuffer);
}

W32Widget::Size W32KeyNameEditor::minimumSize() const {
	return Size(40, 12);
}

LRESULT CALLBACK W32KeyNameEditor::Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	W32KeyNameEditor *editor = ourEditors[hWnd];
	if (editor == 0) {
		return 0;
	}
	int &mask = editor->myKeyboardModifierMask;
	switch (uMsg) {
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;	
		case WM_KEYDOWN:
			if (wParam == 0x10) {
				mask |= 0x1;
			} else if (wParam == 0x11) {
				mask |= 0x2;
			} else if (wParam == 0x12) {
				mask |= 0x4;
			} else {
				editor->setText(ZLKeyUtil::keyName(wParam, wParam, mask));
			}
			return 0;
		case WM_KEYUP:
			if (wParam == 0x10) {
				mask &= ~0x1;
			} else if (wParam == 0x11) {
				mask &= ~0x2;
			} else if (wParam == 0x12) {
				mask &= ~0x4;
			}
			return 0;
		case WM_CHAR:
			return 0;
		case WM_SETFOCUS:
			editor->setText("");
			mask = 0;
		default:
			return editor->myOriginalWndProc(hWnd, uMsg, wParam, lParam);
	}
}

void W32KeyNameEditor::init(HWND parent, W32ControlCollection *collection) {
	W32AbstractEditor::init(parent, collection);
	myOriginalWndProc = (WndProc)SetWindowLong(myWindow, GWL_WNDPROC, (LONG)Callback);
	ourEditors[myWindow] = this;
}

void W32KeyNameEditor::setEditable(bool editable) {
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

W32Widget::Size W32SpinBox::minimumSize() const {
	return Size(36, 12);
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
	W32StandardControl::setPosition(x, y, size);
	if (myControlWindow != 0) {
		RECT r;
		r.left = x + size.Width;
		r.top = y;
		r.right = x + 2 * size.Width;
		r.bottom = y + size.Height;
		MapDialogRect(GetParent(myControlWindow), &r);
		SetWindowPos(myControlWindow, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
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
	W32StandardControl::setVisible(visible);
	if (myControlWindow != 0) {
		ShowWindow(myControlWindow, visible ? SW_SHOW : SW_HIDE);
	}
}

void W32SpinBox::commandCallback(DWORD hiWParam) {
	if (hiWParam == EN_CHANGE) {
		myValue = SendMessage(myControlWindow, UDM_GETPOS, 0, 0);
	}
}

unsigned short W32SpinBox::value() const {
	return myValue;
}

const std::string W32ComboBox::SELECTION_CHANGED_EVENT = "ComboBox: selection changed";
const std::string W32ComboBox::VALUE_EDITED_EVENT = "ComboBox: value edited";

W32ComboBox::W32ComboBox(const std::vector<std::string> &list, int initialIndex) : W32StandardControl(CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP), myList(list), myIndex(initialIndex) {
	if ((initialIndex >= 0) && ((size_t)initialIndex < myList.size())) {
		::createNTWCHARString(myBuffer, list[initialIndex]);
	}
}

W32Widget::Size W32ComboBox::minimumSize() const {
	int len = ((myStyle & CBS_SIMPLE) == CBS_SIMPLE) ? 0 : 10;
	for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
		len = std::max(ZLUnicodeUtil::utf8Length(*it), len);
	}
	return Size(4 * std::min(len, 25), 12);
}

WORD W32ComboBox::classId() const {
	return CLASS_COMBOBOX;
}

void W32ComboBox::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	ZLUnicodeUtil::Ucs2String buffer;
	for (std::vector<std::string>::const_iterator it = myList.begin(); it != myList.end(); ++it) {
		SendMessage(myWindow, CB_ADDSTRING, 0, (LPARAM)::wchar(::createNTWCHARString(buffer, *it)));
	}
	if (myList.size() > 0) {
		SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
		SendMessage(myWindow, CB_SETMINVISIBLE, std::min((int)myList.size(), 7), 0);
	}
}

void W32ComboBox::setPosition(int x, int y, Size size) {
	W32Control::setPosition(x, y, size);
	if (myWindow != 0) {
		SendMessage(myWindow, CB_SETMINVISIBLE, std::min((int)myList.size(), 7), 0);
	}
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

void W32ComboBox::commandCallback(DWORD hiWParam) {
	if (hiWParam == CBN_SELCHANGE) {
		myIndex = SendMessage(myWindow, CB_GETCURSEL, 0, 0);
		const int length = SendMessage(myWindow, CB_GETLBTEXTLEN, myIndex, 0);
		myBuffer.clear();
		myBuffer.insert(myBuffer.end(), length + 1, 0);
		if (length > 0) {
			SendMessage(myWindow, CB_GETLBTEXT, myIndex, (LPARAM)&myBuffer.front());
		}
		fireEvent(SELECTION_CHANGED_EVENT);
	} else if (hiWParam == CBN_EDITCHANGE) {
		const int length = SendMessage(myWindow, WM_GETTEXTLENGTH, 0, 0);
		myBuffer.clear();
		myBuffer.insert(myBuffer.end(), length + 1, 0);
		if (length > 0) {
			SendMessage(myWindow, WM_GETTEXT, length + 1, (LPARAM)&myBuffer.front());
		}
		fireEvent(VALUE_EDITED_EVENT);
	}
}

int W32ComboBox::index() const {
	return myIndex;
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
		if (myList.size() > 0) {
			SendMessage(myWindow, CB_SETMINVISIBLE, std::min((int)myList.size(), 7), 0);
		}
	}
}

void W32ComboBox::setSelection(const std::string &value) {
	std::vector<std::string>::const_iterator it = std::find(myList.begin(), myList.end(), value);
	if (it != myList.end()) {
		setSelection(it - myList.begin());
	}
}

void W32ComboBox::setSelection(int index) {
	if ((index >= 0) && (index < (short)myList.size())) {
		myIndex = index;
		myBuffer.clear();
		::createNTWCHARString(myBuffer, myList[index]);
		if (myWindow != 0) {
			SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
		}
	}
}

W32RadioButton::W32RadioButton(W32RadioButtonGroup &group, const std::string &text) : W32StandardControl(BS_RADIOBUTTON | WS_TABSTOP), myGroup(group), myText(text) {
}

WORD W32RadioButton::classId() const {
	return CLASS_BUTTON;
}

W32Widget::Size W32RadioButton::minimumSize() const {
	return Size(4 * (ZLUnicodeUtil::utf8Length(myText) + 2), 12);
}

void W32RadioButton::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	::setWindowText(myWindow, myText);
}

void W32RadioButton::commandCallback(DWORD hiWParam) {
	if (hiWParam == BN_CLICKED) {
		if (SendMessage(myWindow, BM_GETCHECK, 0, 0) != BST_CHECKED) {
			myGroup.setChecked(*this);
		}
	}
}

void W32RadioButton::setEditable(bool editable) {
	if (editable) {
		myStyle &= ~WS_DISABLED;
	} else {
		myStyle |= WS_DISABLED;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
}

void W32RadioButton::setChecked(bool checked) {
	if (myWindow != 0) {
		SendMessage(myWindow, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
	}
}

W32RadioButtonGroup::W32RadioButtonGroup(const std::string &caption, const std::vector<std::string> &buttonTexts) : W32StandardControl(BS_GROUPBOX), myCaption(caption), myCheckedIndex(-1) {
	myButtons.reserve(buttonTexts.size());
	for (std::vector<std::string>::const_iterator it = buttonTexts.begin(); it != buttonTexts.end(); ++it) {
		myButtons.push_back(new W32RadioButton(*this, *it));
	}
}

WORD W32RadioButtonGroup::classId() const {
	return CLASS_BUTTON;
}

void W32RadioButtonGroup::allocate(WORD *&p, short &id) const {
	W32StandardControl::allocate(p, id);
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->allocate(p, id);
	}
}

int W32RadioButtonGroup::allocationSize() const {
	int size = W32StandardControl::allocationSize();
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		size += (*it)->allocationSize();
	}
	return size;
}

void W32RadioButtonGroup::setEditable(bool editable) {
	if (editable) {
		myStyle &= ~WS_DISABLED;
	} else {
		myStyle |= WS_DISABLED;
	}
	if (myWindow != 0) {
		// TODO: check
		SetWindowLong(myWindow, GWL_STYLE, myStyle);
	}
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		((W32RadioButton&)**it).setEditable(editable);
	}
}

void W32RadioButtonGroup::setVisible(bool visible) {
	W32StandardControl::setVisible(visible);
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		(*it)->setVisible(visible);
	}
}

int W32RadioButtonGroup::controlNumber() const {
	int counter = W32StandardControl::controlNumber();
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		counter += (*it)->controlNumber();
	}
	return counter;
}

void W32RadioButtonGroup::setPosition(int x, int y, Size size) {
	W32StandardControl::setPosition(x, y, size);
	const short deltaY = size.Height / (myButtons.size() + 1);
	size.Width -= 16;
	size.Height = deltaY;
	for (W32WidgetList::iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		y += deltaY;
		(*it)->setPosition(x + 8, y, size)	;
	}
}

W32Widget::Size W32RadioButtonGroup::minimumSize() const {
	Size size;
	for (W32WidgetList::const_iterator it = myButtons.begin(); it != myButtons.end(); ++it) {
		size.Width = std::max(size.Width, (*it)->minimumSize().Width);
	}
	size.Width += 16;
	size.Height = 12 * myButtons.size() + 16;
	return size;
}

void W32RadioButtonGroup::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
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
