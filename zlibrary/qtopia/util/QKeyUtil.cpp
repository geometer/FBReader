/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <cctype>

#include <qobject.h>
#include <qevent.h>

#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLStringUtil.h>

#include "QKeyUtil.h"

std::string QKeyUtil::keyName(QKeyEvent *keyEvent) {
	QString txt = keyEvent->text();
	int key = keyEvent->key();
	int state = keyEvent->state();

	std::string name;
	if ((txt.length() == 1) && txt[0].isPrint() && !txt[0].isSpace()) {
		name = ZLUnicodeUtil::toUpper((const char*)txt.utf8());
	} else {
		name = keyName(key);
	}
	if (name.empty()) {
		std::string last;
		ZLStringUtil::appendNumber(last, state);
		last += "+";
		ZLStringUtil::appendNumber(last, key);
		return last;
	}

	name = '<' + name + '>';
	if (state & 0x20) {
		name = "<Alt>+" + name;
	}
	if (state & 0x10) {
		name = "<Ctrl>+" + name;
	}
	return name;
}

std::string QKeyUtil::keyName(int key) {
	if ((key < 128) && isprint(key) && !isspace(key)) {
		std::string name;
		name += (char)toupper(key);
		return name;
	}
	switch (key) {
		case QObject::Key_Escape:
		case 27:
			return "Esc";
		case QObject::Key_Tab:
			return "Tab";
		case QObject::Key_BackTab:
			return "BackTab";
		case QObject::Key_BackSpace:
			return "BackSpace";
		case QObject::Key_Return:
		case 13:
			return "Return";
		case QObject::Key_Enter:
			return "Enter";
		case QObject::Key_Insert:
			return "Insert";
		case QObject::Key_Delete:
			return "Delete";
		case QObject::Key_Pause:
			return "Pause";
		case QObject::Key_Print:
			return "Print";
		case QObject::Key_SysReq:
			return "SysReq";
		case QObject::Key_Home:
			return "Home";
		case QObject::Key_End:
			return "End";
		case QObject::Key_Left:
			return "LeftArrow";
		case QObject::Key_Up:
			return "UpArrow";
		case QObject::Key_Right:
			return "RightArrow";
		case QObject::Key_Down:
			return "DownArrow";
		case QObject::Key_PageUp:
			return "PageUp";
		case QObject::Key_PageDown:
			return "PageDown";
		case QObject::Key_F1:
			return "F1";
		case QObject::Key_F2:
			return "F2";
		case QObject::Key_F3:
			return "F3";
		case QObject::Key_F4:
			return "F4";
		case QObject::Key_F5:
			return "F5";
		case QObject::Key_F6:
			return "F6";
		case QObject::Key_F7:
			return "F7";
		case QObject::Key_F8:
			return "F8";
		case QObject::Key_F9:
			return "F9";
		case QObject::Key_F10:
			return "F10";
		case QObject::Key_F11:
			return "F11";
		case QObject::Key_F12:
			return "F12";
		case ' ':
			return "Space";
		case QObject::Key_CapsLock:
			return "CapsLock";
		case QObject::Key_NumLock:
			return "NumLock";
		case QObject::Key_ScrollLock:
			return "ScrollLock";
		case 0x200f:
			return "Font -";
		case 0x2010:
			return "Font +";
		case 0x1050:
			return "OK";
	}
	return "";
}
