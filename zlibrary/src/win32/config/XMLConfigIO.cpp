/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "RegistryUtil.h"
#include "AsciiEncoder.h"

#include "XMLConfig.h"

void XMLConfig::load() {
	RegistryUtil util;

	HKEY root;
	HKEY categoryKey;
	HKEY groupKey;
	std::set<std::string> categories;
	std::set<std::string> groups;
	std::set<std::string> valueNames;
	std::string value;

	RegCreateKeyExA(HKEY_CURRENT_USER, util.rootKeyName().c_str(), 0, 0, 0, KEY_ENUMERATE_SUB_KEYS, 0, &root, 0);
	util.collectSubKeys(categories, root);
	for (std::set<std::string>::const_iterator it = categories.begin(); it != categories.end(); ++it) {
		RegCreateKeyExA(root, it->c_str(), 0, 0, 0, KEY_ENUMERATE_SUB_KEYS, 0, &categoryKey, 0);
		groups.clear();
		util.collectSubKeys(groups, categoryKey);
		for (std::set<std::string>::const_iterator jt = groups.begin(); jt != groups.end(); ++jt) {
			valueNames.clear();
			RegCreateKeyExA(categoryKey, jt->c_str(), 0, 0, 0, KEY_QUERY_VALUE, 0, &groupKey, 0);
			XMLConfigGroup *group = getGroup(AsciiEncoder::decode(*jt), true);
			util.collectValues(valueNames, groupKey);
			for (std::set<std::string>::const_iterator kt = valueNames.begin(); kt != valueNames.end(); ++kt) {
				if (util.getValue(value, groupKey, *kt)) {
					group->setValue(AsciiEncoder::decode(*kt), value, *it);
				}
			}
			RegCloseKey(groupKey);
		}
		RegCloseKey(categoryKey);
	}
	RegCloseKey(root);
}
