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
#include <vector>

#include <ZLUnicodeUtil.h>
#include <ZLFile.h>

#include "Migration.h"

#include "../description/BookDescription.h"

Migration_0_8_16::Migration_0_8_16() : Migration("0.8.16") {
}

void Migration_0_8_16::doMigrationInternal() {
	std::vector<std::string> optionGroups;
	ZLOption::listOptionGroups(optionGroups);

	for (std::vector<std::string>::const_iterator it = optionGroups.begin(); it != optionGroups.end(); ++it) {
		if (isLikeToFileName(*it)) {
			ZLFile file(*it);
			if (ZLUnicodeUtil::toLower(file.extension()) == "fb2") {
				BookDescriptionPtr description = BookDescription::getDescription(*it);
				if (!description.isNull()) {
					std::cerr << *it << "\n";
				}
			}
		}
	}
}
