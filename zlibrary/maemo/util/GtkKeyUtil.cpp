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

#include <gdk/gdkkeysyms.h>
#include <gdk/gdkkeys.h>

#include <abstract/ZLUnicodeUtil.h>

#include "GtkKeyUtil.h"

std::string GtkKeyUtil::keyName(GdkEventKey *event) {
	std::string name = keyName(event->keyval);
	if (name.empty()) {
		return "";
	}

	return '<' + name + '>';
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
				return "Escape";
			case GDK_Return:
				return "ScrollKey Center";
			case GDK_Left:
				return "ScrollKey Left";
			case GDK_Up:
				return "ScrollKey Up";
			case GDK_Right:
				return "ScrollKey Right";
			case GDK_Down:
				return "ScrollKey Down";
			case GDK_F4:
				return "Menu";
			case GDK_F5:
				return "Home";
			case GDK_F6:
				return "FullScreen";
			case GDK_F7:
				return "ZoomKey +";
			case GDK_F8:
				return "ZoomKey -";
		}
	}
	return "";
}
