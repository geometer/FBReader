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

#include <ZLStringUtil.h>

#include "../options/FBOptions.h"

#include "Migration.h"

void Migration::moveOption(
	const ZLCategoryKey &oldCategory, const std::string &oldGroup, const std::string &oldName,
	const ZLCategoryKey &newCategory, const std::string &newGroup, const std::string &newName,
	const std::string &defaultValue
) {
	ZLStringOption newOption(newCategory, newGroup, newName, defaultValue);
	const std::string newValue = newOption.value();
	ZLStringOption oldOption(oldCategory, oldGroup, oldName, newValue);
	const std::string oldValue = oldOption.value();
	if (newValue != oldValue) {
		newOption.setValue(oldValue);
		oldOption.setValue(newValue);
	}
}

bool Migration::isLikeToFileName(const std::string &str) {
	return
		ZLStringUtil::stringStartsWith(str, "/") ||
		ZLStringUtil::stringStartsWith(str, "\\\\") ||
		(str.length() > 2) && (str.substr(1, 2) == ":\\");
}

Migration::Migration(const std::string &version) : myVersion(version) {
}

Migration::~Migration() {
}

void Migration::doMigration() {
	ZLStringOption versionOption(FBCategoryKey::SYSTEM, "Version", "FBReaderVersion", "0");
	if (versionOption.value() < myVersion) {
		doMigrationInternal();
	}
}
