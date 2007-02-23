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

#include <iostream>

#include <windows.h>

#include "RegistryUtil.h"
#include "XMLConfigWriter.h"
#include "XMLConfig.h"
#include "AsciiEncoder.h"

const std::string CONFIG = "config";
const std::string _GROUP = "group";
const std::string NAME = "name";
const std::string OPTION = "option";
const std::string VALUE = "value";

void XMLConfigWriter::write() {
	RegistryUtil util;

	const std::string categoryKeyName = util.rootKeyName() + "\\" + myCategory;
	HKEY categoryKey;
	std::set<std::string> existingGroups;
	RegCreateKeyExA(HKEY_CURRENT_USER, categoryKeyName.c_str(), 0, 0, 0, KEY_ENUMERATE_SUB_KEYS | KEY_WRITE, 0, &categoryKey, 0);

	util.collectSubKeys(existingGroups, categoryKey);

	HKEY groupKey;
	std::set<std::string> existingKeys;

	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myConfig.myGroups.begin(); it != myConfig.myGroups.end(); ++it) {
		existingKeys.clear();
		const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
		if (!values.empty()) {
			bool emptyFlag = true;
			for (std::map<std::string,XMLConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
				if (jt->second.Category == myCategory) {
					if (emptyFlag) { 
						std::string groupName = AsciiEncoder::encode(it->first);
						existingGroups.erase(groupName);
						RegCreateKeyExA(categoryKey, groupName.c_str(), 0, 0, 0, KEY_QUERY_VALUE | KEY_WRITE, 0, &groupKey, 0);
						util.collectValues(existingKeys, groupKey);
						emptyFlag = false;
					}
					const std::string name = AsciiEncoder::encode(jt->first);
					const std::string &value = jt->second.Value;
					existingKeys.erase(name);
					RegSetValueExA(groupKey, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.length() + 1);
				}
			}
			if (!emptyFlag) {
				for (std::set<std::string>::const_iterator set_it = existingKeys.begin(); set_it != existingKeys.end(); ++set_it) {
					RegDeleteValueA(groupKey, set_it->c_str());
				}
				RegCloseKey(groupKey);
			}
		}
	}
	for (std::set<std::string>::const_iterator set_it = existingGroups.begin(); set_it != existingGroups.end(); ++set_it) {
		RegDeleteKeyA(categoryKey, set_it->c_str());
	}
	RegCloseKey(categoryKey);
}
