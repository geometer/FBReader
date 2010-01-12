/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLUnicodeUtil.h>

#include "Migration.h"
#include "../options/FBCategoryKey.h"

Migration_0_10_4::Migration_0_10_4() : Migration("0.10.4") {
}

void Migration_0_10_4::doMigrationInternal() {
	std::vector<std::string> groups;
	ZLOption::listOptionGroups(groups);
	for (std::vector<std::string>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
		static const std::string zipPostfix = ".zip";
		static const std::string sizeName = "Size";
		if (ZLStringUtil::stringEndsWith(ZLUnicodeUtil::toLower(*it), zipPostfix)) {
			ZLIntegerOption option(FBCategoryKey::BOOKS, *it, sizeName, -1);
			option.setValue(-1);
		}
	}
}
