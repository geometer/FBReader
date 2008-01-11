/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLOptions.h"

const ZLCategoryKey ZLCategoryKey::EMPTY("");
const ZLCategoryKey ZLCategoryKey::LOOK_AND_FEEL("ui");
const ZLCategoryKey ZLCategoryKey::CONFIG("options");
const ZLCategoryKey ZLCategoryKey::STATE("state");
const ZLCategoryKey ZLCategoryKey::SYSTEM("system");

ZLCategoryKey::ZLCategoryKey(const std::string &name) : Name(name) {
}

bool ZLCategoryKey::operator == (const ZLCategoryKey &key) const {
	return key.Name == Name;
}

bool ZLCategoryKey::operator != (const ZLCategoryKey &key) const {
	return !operator==(key);
}
