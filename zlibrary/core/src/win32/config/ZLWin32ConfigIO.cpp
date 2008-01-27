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

#include "StringEncoder.h"

#include "ZLWin32Config.h"

void ZLWin32Config::load() {
	HKEY root;
	HKEY categoryKey;
	HKEY groupKey;
	std::set<std::string> categories;
	std::set<std::string> groups;
	std::set<std::string> valueNames;
	std::string value;

	RegCreateKeyExA(HKEY_CURRENT_USER, rootKeyName().c_str(), 0, 0, 0, KEY_ENUMERATE_SUB_KEYS, 0, &root, 0);
	collectSubKeys(categories, root);
	for (std::set<std::string>::const_iterator it = categories.begin(); it != categories.end(); ++it) {
		RegCreateKeyExA(root, it->c_str(), 0, 0, 0, KEY_ENUMERATE_SUB_KEYS, 0, &categoryKey, 0);
		groups.clear();
		collectSubKeys(groups, categoryKey);
		for (std::set<std::string>::const_iterator jt = groups.begin(); jt != groups.end(); ++jt) {
			valueNames.clear();
			RegCreateKeyExA(categoryKey, jt->c_str(), 0, 0, 0, KEY_QUERY_VALUE, 0, &groupKey, 0);
			collectValues(valueNames, groupKey);
			std::set<std::string>::iterator nameIt = valueNames.find(REAL_GROUPNAME_KEY);
			std::string groupName = *jt;
			if (nameIt != valueNames.end()) {
				valueNames.erase(nameIt);
				registryGetValue(groupName, groupKey, REAL_GROUPNAME_KEY);
				myGroupAliases[groupName] = *jt;
				myPseudoGroupNameNumber = std::max(myPseudoGroupNameNumber, atoi(jt->substr(4).c_str()) + 1);
			}
			ZLWin32ConfigGroup *group = getGroup(StringEncoder::decode(groupName), true);
			for (std::set<std::string>::const_iterator kt = valueNames.begin(); kt != valueNames.end(); ++kt) {
				if (registryGetValue(value, groupKey, *kt)) {
					group->setValue(StringEncoder::decode(*kt), value, *it);
				}
			}
			RegCloseKey(groupKey);
		}
		RegCloseKey(categoryKey);
	}
	RegCloseKey(root);
}
