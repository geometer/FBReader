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

#include <ZLTime.h>

#include "ZLWin32Config.h"
#include "StringEncoder.h"

void ZLWin32ConfigManager::createInstance() {
	ourInstance = new ZLWin32ConfigManager();
}

ZLWin32ConfigManager::ZLWin32ConfigManager() {
}

ZLConfig *ZLWin32ConfigManager::createConfig() const {
	ZLConfig *config = new ZLWin32Config();
	ourIsInitialised = true;
	return config;
}

ZLWin32ConfigGroup::ZLWin32ConfigGroup(const std::string &groupName, std::set<std::string> &categories) : myCategories(categories) {
	myName = StringEncoder::encode(groupName);
}

void ZLWin32ConfigGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,ZLWin32ConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		if (category == it->second.Category) {
			if (it->second.Value != value) {
				it->second.Value = value;
			}
			return;
		} else {
			myValues.erase(it);
		}
	}
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	myValues.insert(std::pair<std::string,ZLWin32ConfigValue>(name, ZLWin32ConfigValue(*jt, value)));
}

ZLWin32Config::ZLWin32Config() : myBufferSize(4096), myPseudoGroupNameNumber(0) {
	myBuffer = new char[myBufferSize];
	load();
}

ZLWin32Config::~ZLWin32Config() {
	for (std::map<std::string,ZLWin32ConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
	delete[] myBuffer;
}

ZLWin32ConfigGroup *ZLWin32Config::getGroup(const std::string &name, bool createUnexisting) {
	std::map<std::string,ZLWin32ConfigGroup*>::const_iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		return it-> second;
	}
	if (createUnexisting) {
		ZLWin32ConfigGroup *group = new ZLWin32ConfigGroup(name, myCategories);
		myGroups.insert(std::pair<std::string,ZLWin32ConfigGroup*>(name, group));
		return group;
	}
	return 0;
}

void ZLWin32Config::listOptionNames(const std::string &groupName, std::vector<std::string> &names) {
	std::map<std::string,ZLWin32ConfigGroup*>::const_iterator it = myGroups.find(groupName);
	if (it == myGroups.end()) {
		return;
	}
	const std::map<std::string,ZLWin32ConfigValue> &values = it->second->myValues;
	for (std::map<std::string,ZLWin32ConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
		names.push_back(jt->first);
	}
}

void ZLWin32Config::listOptionGroups(std::vector<std::string> &groups) {
	for (std::map<std::string,ZLWin32ConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		groups.push_back(it->first);
	}
}

void ZLWin32Config::removeGroup(const std::string &name) {
	std::map<std::string,ZLWin32ConfigGroup*>::iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		HKEY key;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, rootKeyName().c_str(), 0, KEY_WRITE, &key) == ERROR_SUCCESS) {
			for (std::set<std::string>::const_iterator jt = myCategories.begin(); jt != myCategories.end(); ++jt) {
				RegDeleteKeyA(key, (*jt + "\\" + it->second->myName).c_str()); 
			}
			RegCloseKey(key);
		}
		delete it->second;
		myGroups.erase(it);
	}
}

void ZLWin32Config::setValue(const std::string &groupName, const std::string &name, const std::string &value, const std::string &category) {
	ZLWin32ConfigGroup *group = getGroup(groupName, true);

	std::map<std::string,ZLWin32ConfigValue> &groupValues = group->myValues;
	std::map<std::string,ZLWin32ConfigValue>::iterator it = groupValues.find(name);
	if (it != groupValues.end()) {
		if (category == it->second.Category) {
			if (it->second.Value != value) {
				it->second.Value = value;
				registrySetValue(category, group->myName, name, value);
			}
			return;
		} else {
			registryRemoveValue(it->second.Category, group->myName, name);
			groupValues.erase(it);
		}
	}
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	registrySetValue(category, group->myName, name, value);
	groupValues.insert(std::pair<std::string,ZLWin32ConfigValue>(name, ZLWin32ConfigValue(*jt, value)));
}

void ZLWin32Config::unsetValue(const std::string &groupName, const std::string &name) {
	ZLWin32ConfigGroup *group = getGroup(groupName, false);
	if (group != 0) {
		std::map<std::string,ZLWin32ConfigValue> &values = group->myValues;
		std::map<std::string,ZLWin32ConfigValue>::iterator it = values.find(name);
		if (it != values.end()) {
			registryRemoveValue(it->second.Category, group->myName, name);
			values.erase(it);
		}
	}
}

const std::string &ZLWin32Config::getValue(const std::string &groupName, const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,ZLWin32ConfigGroup*>::const_iterator it = myGroups.find(groupName);
	if (it == myGroups.end()) {
		return defaultValue;
	}
	const std::map<std::string,ZLWin32ConfigValue> &values = it->second->myValues;
	std::map<std::string,ZLWin32ConfigValue>::const_iterator jt = values.find(name);
	return (jt != values.end()) ? jt->second.Value : defaultValue;
}

const std::string &ZLWin32Config::getDefaultValue(const std::string&, const std::string&, const std::string &defaultValue) const {
	return defaultValue;
}

bool ZLWin32Config::isAutoSavingSupported() const {
	return false;
}

void ZLWin32Config::startAutoSave(int) {
}
