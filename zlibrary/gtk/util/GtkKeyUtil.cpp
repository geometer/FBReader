/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <gdk/gdkkeysyms.h>
#include <gdk/gdkkeys.h>

#include <abstract/ZLUnicodeUtil.h>

#include "GtkKeyUtil.h"

std::string GtkKeyUtil::keyName(GdkEventKey *event) {
	std::string name = keyName(event->keyval);
	if (name.empty()) {
		return "";
	}

	name = '<' + name + '>';
	if (event->state & 0x8) {
		name = "<Alt>+" + name;
	}
	if (event->state & 0x4) {
		name = "<Ctrl>+" + name;
	}
	return name;
}

std::string GtkKeyUtil::keyName(int keyval) {
	ZLUnicodeUtil::Ucs2Char chr = gdk_keyval_to_unicode(keyval);
	if (((chr < 128) && isprint(chr) && !isspace(chr)) || ZLUnicodeUtil::isLetter(chr)) {
		char buf[3];
		std::string name;
		name.append(buf, ZLUnicodeUtil::ucs2ToUtf8(buf, ZLUnicodeUtil::toUpper(chr)));
		return name;
	} else {
		switch (keyval) {
			case GDK_Escape:
				return "Esc";
			case GDK_Tab:
				return "Tab";
			case GDK_BackSpace:
				return "BackSpace";
			case GDK_Return:
				return "Return";
			case GDK_Insert:
				return "Insert";
			case GDK_Delete:
				return "Delete";
			case GDK_Pause:
				return "Pause";
			case GDK_Print:
				return "Print";
			case GDK_Sys_Req:
				return "SysReq";
			case GDK_Clear:
				return "Clear";
			case GDK_Home:
				return "Home";
			case GDK_End:
				return "End";
			case GDK_Left:
				return "LeftArrow";
			case GDK_Up:
				return "UpArrow";
			case GDK_Right:
				return "RightArrow";
			case GDK_Down:
				return "DownArrow";
			case GDK_Page_Up:
				return "PageUp";
			case GDK_Page_Down:
				return "PageDown";
			case GDK_F1:
				return "F1";
			case GDK_F2:
				return "F2";
			case GDK_F3:
				return "F3";
			case GDK_F4:
				return "F4";
			case GDK_F5:
				return "F5";
			case GDK_F6:
				return "F6";
			case GDK_F7:
				return "F7";
			case GDK_F8:
				return "F8";
			case GDK_F9:
				return "F9";
			case GDK_F10:
				return "F10";
			case GDK_F11:
				return "F11";
			case GDK_F12:
				return "F12";
			case ' ':
				return "Space";
			case GDK_Caps_Lock:
				return "CapsLock";
			case GDK_Num_Lock:
				return "NumLock";
			case GDK_Scroll_Lock:
				return "ScrollLock";
		}
	}
	return "";
}
