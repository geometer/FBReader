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

#include <ZLTime.h>

#include "XMLConfig.h"
#include "RegistryUtil.h"
#include "AsciiEncoder.h"

static RegistryUtil util;

XMLConfigGroup::XMLConfigGroup(const std::string &groupName, std::set<std::string> &categories) : myCategories(categories) {
	myName = AsciiEncoder::encode(groupName);
}

void XMLConfigGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
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
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
}

void XMLConfigGroup::setValue1(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		if (category == it->second.Category) {
			if (it->second.Value != value) {
				it->second.Value = value;
				util.setValue(category + "\\" + myName, name, value);
			}
			return;
		} else {
			util.removeValue(it->second.Category + "\\" + myName, name);
			myValues.erase(it);
		}
	}
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	util.setValue(category + "\\" + myName, name, value);
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
}

XMLConfig::XMLConfig() {
	load();
}

XMLConfig::~XMLConfig() {
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
}

XMLConfigGroup *XMLConfig::getGroup(const std::string &name, bool createUnexisting) {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		return it-> second;
	}
	if (createUnexisting) {
		XMLConfigGroup *group = new XMLConfigGroup(name, myCategories);
		myGroups.insert(std::pair<std::string,XMLConfigGroup*>(name, group));
		return group;
	}
	return 0;
}

void XMLConfig::removeGroup(const std::string &name) {
	std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		HKEY key;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, util.rootKeyName().c_str(), 0, KEY_WRITE, &key) == ERROR_SUCCESS) {
			for (std::set<std::string>::const_iterator jt = myCategories.begin(); jt != myCategories.end(); ++jt) {
				RegDeleteKeyA(key, (*jt + "\\" + it->second->myName).c_str()); 
			}
			RegCloseKey(key);
		}
		delete it->second;
		myGroups.erase(it);
	}
}

void XMLConfig::setValue(const std::string &groupName, const std::string &name, const std::string &value, const std::string &category) {
	XMLConfigGroup *group = getGroup(groupName, true);

	std::map<std::string,XMLConfigValue> &groupValues = group->myValues;
	std::map<std::string,XMLConfigValue>::iterator it = groupValues.find(name);
	if (it != groupValues.end()) {
		if (category == it->second.Category) {
			if (it->second.Value != value) {
				it->second.Value = value;
				util.setValue(category + "\\" + group->myName, name, value);
			}
			return;
		} else {
			util.removeValue(it->second.Category + "\\" + group->myName, name);
			groupValues.erase(it);
		}
	}
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	util.setValue(category + "\\" + group->myName, name, value);
	groupValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
}

void XMLConfig::unsetValue(const std::string &groupName, const std::string &name) {
	XMLConfigGroup *group = getGroup(groupName, false);
	if (group != 0) {
		std::map<std::string,XMLConfigValue> &values = group->myValues;
		std::map<std::string,XMLConfigValue>::iterator it = values.find(name);
		if (it != values.end()) {
			util.removeValue(it->second.Category + "\\" + group->myName, name);
			values.erase(it);
		}
	}
}

const std::string &XMLConfig::getValue(const std::string &groupName, const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.find(groupName);
	if (it == myGroups.end()) {
		return defaultValue;
	}
	const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
	std::map<std::string,XMLConfigValue>::const_iterator jt = values.find(name);
	return (jt != values.end()) ? jt->second.Value : defaultValue;
}
