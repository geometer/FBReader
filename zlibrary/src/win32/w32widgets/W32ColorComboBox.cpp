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

#include "W32ColorComboBox.h"

static const WORD CLASS_COMBOBOX = 0x0085;

const std::string W32ColorComboBox::SELECTION_CHANGED_EVENT = "ColorComboBox: selection changed";
std::vector<std::string> W32ColorComboBox::ourStrings;
std::vector<ZLColor> W32ColorComboBox::ourColors;

W32ColorComboBox::W32ColorComboBox(ZLColor initialColor) : W32StandardControl(CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_TABSTOP), myUseCustomColor(false) {
	setSelectedColor(initialColor);
}

ZLColor W32ColorComboBox::selectedColor() const {
	return color(myIndex);
}

void W32ColorComboBox::setSelectedColor(ZLColor color) {
	initVectors();
	myIndex = -1;
	for (std::vector<ZLColor>::const_iterator it = ourColors.begin(); it != ourColors.end(); ++it) {
		if (*it == color) {
			myIndex = it - ourColors.begin();
			break;
		}
	}
	bool customColorWasNotUsed = !myUseCustomColor;
	if (myIndex == -1) {
		myUseCustomColor = true;
		myCustomColor = color;
	}
	if (myUseCustomColor) {
		++myIndex;
		if (customColorWasNotUsed && (myWindow != 0)) {
			SendMessageA(myWindow, CB_ADDSTRING, 0, (LPARAM)"");
		}
	}
	if (myWindow != 0) {
		SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
	}
}

void W32ColorComboBox::removeCustomColor() {
	// TODO: implement
	/*
	if (myUseCustomColor) {
		if (myIndex > 0) {
			--myIndex;
		}
		myUseCustomColor = false;
		if (myWindow != 0) {
			SendMessage(myWindow, CB_DELETESTRING, 0, 0);
			SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
		}
	}
	*/
}

W32Widget::Size W32ColorComboBox::minimumSize() const {
	return Size(40, 12);
}

WORD W32ColorComboBox::classId() const {
	return CLASS_COMBOBOX;
}

void W32ColorComboBox::init(HWND parent, W32ControlCollection *collection) {
	W32StandardControl::init(parent, collection);
	ZLUnicodeUtil::Ucs2String buffer;
	for (int i = 0; i < (myUseCustomColor ? 17 : 16); ++i) {
		SendMessageA(myWindow, CB_ADDSTRING, 0, (LPARAM)"");
	}
	SendMessage(myWindow, CB_SETCURSEL, myIndex, 0);
	SendMessage(myWindow, CB_SETMINVISIBLE, 7, 0);

	RECT rectangle;
	rectangle.left = 0;
	rectangle.top = 0;
	rectangle.right = mySize.Width;
	rectangle.bottom = mySize.Height;
	MapDialogRect(parent, &rectangle);
	myPixelWidth = rectangle.right;
	myPixelHeight = rectangle.bottom;
}

void W32ColorComboBox::commandCallback(DWORD hiWParam) {
	if (hiWParam == CBN_SELCHANGE) {
		myIndex = SendMessage(myWindow, CB_GETCURSEL, 0, 0);
		fireEvent(SELECTION_CHANGED_EVENT);
	}
}

void W32ColorComboBox::drawItemCallback(DRAWITEMSTRUCT &di) {
	RECT rectangle;
	const int height = myPixelHeight - 4;
	rectangle.top = di.rcItem.top + 2;
	rectangle.bottom = di.rcItem.bottom - 2;
	rectangle.left = di.rcItem.left + 2;
	rectangle.right = rectangle.left + 3 * (myPixelHeight - 4) / 2;
	MoveToEx(di.hDC, rectangle.left, rectangle.top, 0);
	LineTo(di.hDC, rectangle.left, rectangle.bottom);
	LineTo(di.hDC, rectangle.right, rectangle.bottom);
	LineTo(di.hDC, rectangle.right, rectangle.top);
	LineTo(di.hDC, rectangle.left, rectangle.top);
	++rectangle.top;
	++rectangle.left;
	ZLColor color = this->color(di.itemID);
	HBRUSH brush = CreateSolidBrush(RGB(color.Red, color.Green, color.Blue));
	FillRect(di.hDC, &rectangle, brush);
	DeleteObject(brush);

	TEXTMETRIC tm;
	const std::string &txt = text(di.itemID);
	GetTextMetrics(di.hDC, &tm);
	TextOutA(di.hDC, rectangle.right + 8, (rectangle.top + rectangle.bottom - tm.tmHeight) / 2, txt.data(), txt.length());
}

void W32ColorComboBox::addColor(const std::string &name, ZLColor color) {
	ourStrings.push_back(name);
	ourColors.push_back(color);
}

void W32ColorComboBox::initVectors() {
	if (ourStrings.empty()) {
		addColor("Black", ZLColor(0, 0, 0));
		addColor("White", ZLColor(255, 255, 255));
		addColor("Maroon", ZLColor(128, 0, 0));
		addColor("Green", ZLColor(0, 128, 0));
		addColor("Olive", ZLColor(128, 128, 0));
		addColor("Navy", ZLColor(0, 0, 128));
		addColor("Purple", ZLColor(128, 0, 128));
		addColor("Teal", ZLColor(0, 128, 128));
		addColor("Silver", ZLColor(192, 192, 192));
		addColor("Gray", ZLColor(128, 128, 128));
		addColor("Red", ZLColor(255, 0, 0));
		addColor("Lime", ZLColor(0, 255, 0));
		addColor("Yellow", ZLColor(255, 255, 0));
		addColor("Blue", ZLColor(0, 0, 255));
		addColor("Magenta", ZLColor(255, 0, 255));
		addColor("Cyan", ZLColor(0, 255, 255));
	}
}

const std::string &W32ColorComboBox::text(int index) const {
	initVectors();
	if (myUseCustomColor) {
		if (index == 0) {
			static const std::string CUSTOM = "Custom";
			return CUSTOM;
		}
		--index;
	}
	return ourStrings[index];
}

const ZLColor W32ColorComboBox::color(int index) const {
	initVectors();
	if (myUseCustomColor) {
		if (index == 0) {
			return myCustomColor;
		}
		--index;
	}
	return ourColors[index];
}
