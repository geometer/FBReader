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
	return ZLKeyUtil::keyName(gdk_keyval_to_unicode(event->keyval), event->keyval, event->state);
}

	/*
std::string GtkKeyUtil::keyName(int keyval) {
	ZLUnicodeUtil::Ucs2Char chr = gdk_keyval_to_unicode(keyval);
	if (((chr < 128) && isprint(chr) && !isspace(chr)) || ZLUnicodeUtil::isLetter(chr)) {
		char buf[3];
		std::string name;
		name.append(buf, ZLUnicodeUtil::ucs2ToUtf8(buf, ZLUnicodeUtil::toUpper(chr)));
		return name;
	} else {
		switch (keyval) {
			case GDK_Home:
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
	*/
