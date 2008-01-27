/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLStringUtil.h>

#include "ZLWin32Config.h"

const std::string ZLWin32Config::PSEUDO_GROUPNAME_PREFIX = "____";
const std::string ZLWin32Config::REAL_GROUPNAME_KEY = "____realGroupName";

std::string ZLWin32Config::rootKeyName() const {
	return "Software\\" + ZLibrary::ApplicationName();
}

void ZLWin32Config::collectSubKeys(std::set<std::string> &keySet, HKEY root) {
	DWORD index = 0;
	DWORD len = myBufferSize;
	while (RegEnumKeyExA(root, index, myBuffer, &len, 0, 0, 0, 0) == ERROR_SUCCESS) {
		keySet.insert(myBuffer);
		++index;
		len = myBufferSize;
	}
}

void ZLWin32Config::collectValues(std::set<std::string> &valueSet, HKEY key) {
	DWORD index = 0;
	DWORD len = myBufferSize;
	while (RegEnumValueA(key, index, myBuffer, &len, 0, 0, 0, 0) == ERROR_SUCCESS) {
		valueSet.insert(myBuffer);
		++index;
		len = myBufferSize;
	}
}

bool ZLWin32Config::registryGetValue(std::string &value, HKEY key, const std::string &name) {
	DWORD size = myBufferSize;
	DWORD type;
	if ((RegQueryValueExA(key, name.c_str(), 0, &type, (BYTE*)myBuffer, &size) != ERROR_SUCCESS) ||
			(type != REG_SZ)) {
		return false;
	}
	value = myBuffer;
	return true;
}

void ZLWin32Config::registryRemoveValue(const std::string &category, const std::string &group, const std::string &valueName) {
	HKEY key;
	const std::string fullKeyName = rootKeyName() + "\\" + category + "\\" + group;
	LONG code = RegOpenKeyExA(HKEY_CURRENT_USER, fullKeyName.c_str(), 0, KEY_WRITE, &key);
	if (code != ERROR_SUCCESS) {
		std::string groupAlias = myGroupAliases[group];
		if (!groupAlias.empty()) {
			const std::string alternateKeyName = rootKeyName() + "\\" + category + "\\" + groupAlias;
			code = RegOpenKeyExA(HKEY_CURRENT_USER, alternateKeyName.c_str(), 0, KEY_WRITE, &key);
		}
	}
	if (code == ERROR_SUCCESS) {
		RegDeleteValueA(key, valueName.c_str());
		RegCloseKey(key);
		// TODO: remove empty group
	}
}

void ZLWin32Config::registrySetValue(const std::string &category, const std::string &group, const std::string &valueName, const std::string &value) {
	const std::string fullKeyName = rootKeyName() + "\\" + category + "\\" + group;
	HKEY key;
	LONG code = RegCreateKeyExA(HKEY_CURRENT_USER, fullKeyName.c_str(), 0, 0, 0, KEY_WRITE, 0, &key, 0);
	if (code != ERROR_SUCCESS) {
		std::string groupAlias = myGroupAliases[group];
		if (groupAlias.empty()) {
			groupAlias = PSEUDO_GROUPNAME_PREFIX;
			ZLStringUtil::appendNumber(groupAlias, myPseudoGroupNameNumber++);
			myGroupAliases[group] = groupAlias;
		}
		const std::string alternateKeyName = rootKeyName() + "\\" + category + "\\" + groupAlias;
		code = RegCreateKeyExA(HKEY_CURRENT_USER, alternateKeyName.c_str(), 0, 0, 0, KEY_WRITE, 0, &key, 0);
		if (code == ERROR_SUCCESS) {
			RegSetValueExA(key, REAL_GROUPNAME_KEY.c_str(), 0, REG_SZ, (const BYTE*)group.c_str(), group.length() + 1);
		}
	}
	if (code == ERROR_SUCCESS) {
		RegSetValueExA(key, valueName.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.length() + 1);
		RegCloseKey(key);
	}
}
