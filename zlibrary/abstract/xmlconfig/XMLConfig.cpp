/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "XMLConfig.h"
#include "XMLConfigDelta.h"

const std::string &XMLConfigGroup::getValue(const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,XMLConfigValue>::const_iterator it = myValues.find(name);
	return (it != myValues.end()) ? it->second.Value : defaultValue;
}

void XMLConfigGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	bool insertFlag = true;
	if (it != myValues.end()) {
		if (it->second.Category == category) {
			it->second.Value = value;
			insertFlag = false;
		} else {
			myValues.erase(it);
		}
	}
	if (insertFlag) {
		std::set<std::string>::iterator jt = myCategories.find(category);
		if (jt == myCategories.end()) {
			jt = myCategories.insert(category).first;
		}
		myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
	}
}

void XMLConfigGroup::setCategory(const std::string &name, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it == myValues.end() || (it->second.Category == category)) {
		return;
	}
	std::string value = it->second.Value;
	myValues.erase(it);
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
}

void XMLConfigGroup::unsetValue(const std::string &name) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		myValues.erase(it);
	}
}

XMLConfig::XMLConfig(const std::string &name, const std::string &homeDirectory) : myHomeDirectory(homeDirectory), myName(name), myDelta(0) {
	load();
	myDelta = new XMLConfigDelta(myCategories);
}

XMLConfig::~XMLConfig() {
	saveDelta();
	saveAll();
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
	if (myDelta != 0) {
		delete myDelta;
	}
}

XMLConfigGroup *XMLConfig::getGroup(const std::string &name, bool createUnexisting) {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		return it-> second;
	}
	if (createUnexisting) {
		XMLConfigGroup *group = new XMLConfigGroup(myCategories);
		myGroups.insert(std::pair<std::string,XMLConfigGroup*>(name, group));
		return group;
	}
	return 0;
}

XMLConfigGroup *XMLConfig::getGroup(const std::string &name) const {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.find(name);
	return (it != myGroups.end()) ? it-> second : 0;
}

void XMLConfig::removeGroup(const std::string &name) {
	std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		delete it->second;
		myGroups.erase(it);
	}
}
