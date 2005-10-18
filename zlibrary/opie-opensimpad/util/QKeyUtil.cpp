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

#include "QKeyUtil.h"

std::string QKeyUtil::keyName(QKeyEvent *keyEvent) {
	QString txt = keyEvent->text();
	int key = keyEvent->key();
	int state = keyEvent->state();

	std::string name = (const char*)txt.upper().utf8();
	if (name.empty() || ((name.length() == 1) && !isprint(name[0]) || isspace(name[0]))) {
		name = keyName(key);
	}
	if (name.empty()) {
		return "";
	}

	name = '<' + name + '>';
	if (state & 0x400) {
		name = "<Alt>+" + name;
	}
	if (state & 0x200) {
		name = "<Ctrl>+" + name;
	}
	return name;
}

std::string QKeyUtil::keyName(int key) {
	if ((key < 128) && isprint(key) && (!isspace(key)) && (key != '&')) {
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
			return "OK";
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
			return "Left";
		case QObject::Key_Up:
			return "Up";
		case QObject::Key_Right:
			return "Right";
		case QObject::Key_Down:
			return "Down";
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
			return "Esc>+<Up";
		case QObject::Key_F6:
			return "Esc>+<Down";
		case QObject::Key_F7:
			return "Esc>+<Right";
		case QObject::Key_F8:
			return "Esc>+<Left";
		case QObject::Key_F9:
			return "OK>+<Up";
		case QObject::Key_F10:
			return "OK>+<Down";
		case QObject::Key_F11:
			return "OK>+<Right";
		case 4156:
			return "OK>+<Left";
		case QObject::Key_F12:
			return "F12";
		case ' ':
			return "Space";
		case '&':
			return "&&";
		case QObject::Key_CapsLock:
			return "CapsLock";
		case QObject::Key_NumLock:
			return "NumLock";
		case QObject::Key_ScrollLock:
			return "ScrollLock";
	}
	return "";
}
