/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLApplication.h>

#include "RegistryUtil.h"

RegistryUtil::RegistryUtil() : myBufferSize(4096) {
	myBuffer = new char[myBufferSize];
}

RegistryUtil::~RegistryUtil() {
	delete[] myBuffer;
}

std::string RegistryUtil::rootKeyName() const {
	return "Software\\" + ZLApplication::ApplicationName();
}

void RegistryUtil::collectSubKeys(std::set<std::string> &keySet, HKEY root) {
	DWORD index = 0;
	DWORD len = myBufferSize;
	while (RegEnumKeyExA(root, index, myBuffer, &len, 0, 0, 0, 0) == ERROR_SUCCESS) {
		keySet.insert(myBuffer);
		++index;
		len = myBufferSize;
	}
}

void RegistryUtil::collectValues(std::set<std::string> &valueSet, HKEY key) {
	DWORD index = 0;
	DWORD len = myBufferSize;
	while (RegEnumValueA(key, index, myBuffer, &len, 0, 0, 0, 0) == ERROR_SUCCESS) {
		valueSet.insert(myBuffer);
		++index;
		len = myBufferSize;
	}
}

bool RegistryUtil::getValue(std::string &value, HKEY key, const std::string &name) {
	DWORD size = myBufferSize;
	DWORD type;
	if ((RegQueryValueExA(key, name.c_str(), 0, &type, (BYTE*)myBuffer, &size) != ERROR_SUCCESS) ||
			(type != REG_SZ)) {
		return false;
	}
	value = myBuffer;
	return true;
}

void RegistryUtil::removeValue(const std::string &keyName, const std::string &valueName) {
	const std::string fullKeyName = rootKeyName() + "\\" + keyName;
	HKEY key;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, fullKeyName.c_str(), 0, KEY_WRITE, &key) == ERROR_SUCCESS) {
		RegDeleteValueA(key, valueName.c_str());
		RegCloseKey(key);
		// TODO: remove empty group
	}
}

void RegistryUtil::setValue(const std::string &keyName, const std::string &valueName, const std::string &value) {
	const std::string fullKeyName = rootKeyName() + "\\" + keyName;
	HKEY key;
	if (RegCreateKeyExA(HKEY_CURRENT_USER, fullKeyName.c_str(), 0, 0, 0, KEY_WRITE, 0, &key, 0) == ERROR_SUCCESS) {
		RegSetValueExA(key, valueName.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.length() + 1);
		RegCloseKey(key);
	}
}
