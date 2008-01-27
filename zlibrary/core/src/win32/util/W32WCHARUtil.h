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

#ifndef __W32WCHARUTIL_H__
#define __W32WCHARUTIL_H__

#include <windows.h>

#include <ZLUnicodeUtil.h>

inline const WCHAR *wchar(const ZLUnicodeUtil::Ucs2String &str) {
	return (const WCHAR*)&str.front();
}

inline const ZLUnicodeUtil::Ucs2String &createNTWCHARString(ZLUnicodeUtil::Ucs2String &str, const std::string &utf8) {
	str.clear();
	ZLUnicodeUtil::utf8ToUcs2(str, utf8.data(), utf8.length());
	str.push_back(0);
	return str;
}

inline void setWindowText(HWND window, const std::string &text) {
	if (window != 0) {
		ZLUnicodeUtil::Ucs2String str;
		SetWindowTextW(window, ::wchar(::createNTWCHARString(str, text)));
	}
}

#endif /* __W32WCHARUTIL_H__ */
