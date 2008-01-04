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

#include <ZLPaintContext.h>

#include "ZLFontFamilyOptionEntry.h"

ZLFontFamilyOptionEntry::ZLFontFamilyOptionEntry(ZLStringOption &option, const ZLPaintContext &context) : myOption(option), myContext(context) {
	std::string value = option.value();
	if (!value.empty()) {
		option.setValue(myContext.realFontFamilyName(value));
	}
}

const std::vector<std::string> &ZLFontFamilyOptionEntry::values() const {
	return myContext.fontFamilies();
}

const std::string &ZLFontFamilyOptionEntry::initialValue() const {
	return myOption.value();
}

void ZLFontFamilyOptionEntry::onAccept(const std::string &value) {
	myOption.setValue(value);
}
