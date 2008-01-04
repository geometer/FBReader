/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>

#include "ZLStringInfoEntry.h"

ZLStringInfoEntry::ZLStringInfoEntry(const std::string &value) : myValue(value) {
	setActive(false);
}

ZLStringInfoEntry::ZLStringInfoEntry(int value) {
	ZLStringUtil::appendNumber(myValue, value);
	setActive(false);
}

const std::string &ZLStringInfoEntry::initialValue() const {
	return myValue;
}

void ZLStringInfoEntry::onAccept(const std::string&) {
}
