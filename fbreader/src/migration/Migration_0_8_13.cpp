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

#include <vector>

#include <ZLStringUtil.h>

#include "Migration.h"

Migration_0_8_13::Migration_0_8_13() : Migration("0.8.13") {
}

void Migration_0_8_13::doMigrationInternal() {
	std::vector<std::string> optionNames;
	ZLOption::listOptionNames("Style", optionNames);
	for (std::vector<std::string>::const_iterator it = optionNames.begin(); it != optionNames.end(); ++it) {
		if (ZLStringUtil::stringEndsWith(*it, ":lineSpacing") ||
				ZLStringUtil::stringEndsWith(*it, ":lineSpace")) {
			ZLDoubleOption doubleOption(ZLCategoryKey::LOOK_AND_FEEL, "Style", *it, 0.0);
			ZLIntegerOption intOption(ZLCategoryKey::LOOK_AND_FEEL, "Style", *it + "Percent", -1);
			const double doubleValue = doubleOption.value();
			const int intValue = intOption.value();
			doubleOption.setValue((intValue == -1) ? 0.0 : (intValue / 100.0));
			intOption.setValue((int)(doubleValue * 100));
		}
	}
}
