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

#include <ZLUnicodeUtil.h>

#include "W32Element.h"

W32ControlCollection::W32ControlCollection(int startId) : myCurrentId(startId) {
}

short W32ControlCollection::addControl(W32Control *control) {
	myControlByIdMap[myCurrentId] = control;
	return myCurrentId++;
}

W32Control *W32ControlCollection::operator[] (short id) {
	std::map<short,W32Control*>::iterator it = myControlByIdMap.find(id);
	return (it != myControlByIdMap.end()) ? it->second : 0;
}

W32Element::W32Element() {
}

W32Element::~W32Element() {
}

void W32Element::allocateString(WORD *&p, const std::string &text) {
	ZLUnicodeUtil::Ucs2String ucs2Str;
	ZLUnicodeUtil::utf8ToUcs2(ucs2Str, text.data(), text.length());
	ucs2Str.push_back(0);
	memcpy(p, &ucs2Str.front(), 2 * ucs2Str.size());
	p += ucs2Str.size();
}
