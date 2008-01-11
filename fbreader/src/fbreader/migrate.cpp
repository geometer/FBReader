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

#include <iostream>

#include <ZLOptions.h>
#include "../options/FBOptions.h"

#include "migrate.h"

static void moveOption(
	const ZLCategoryKey &oldCategory, const std::string &oldGroup, const std::string &oldName,
	const ZLCategoryKey &newCategory, const std::string &newGroup, const std::string &newName,
	const std::string &defaultValue
) {
	ZLStringOption oldOption(oldCategory, oldGroup, oldName, defaultValue);
	ZLStringOption newOption(newCategory, newGroup, newName, defaultValue);
	if (newOption.value() != oldOption.value()) {
		newOption.setValue(oldOption.value());
		oldOption.setValue(defaultValue);
	}
}

void migrateTo_0_8_11() {
	moveOption(
		ZLCategoryKey::CONFIG, "Options", "ScrollingDelay",
		ZLCategoryKey::CONFIG, "LargeScrolling", "ScrollingDelay",
		"250"	
	);
}

void migrateFromOldVersions() {
	ZLStringOption versionOption(FBCategoryKey::SYSTEM, "Version", "FBReaderVersion", "0");
	const std::string version = versionOption.value();

	if (version < "0.8.11") {
		migrateTo_0_8_11();
	}

	versionOption.setValue("0.8.11");
}
