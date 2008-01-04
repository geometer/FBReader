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

#include "XMLConfigDelta.h"

void XMLConfigDelta::addCategory(const std::string &category) {
	if (myCategories.find(category) == myCategories.end()) {
		myCategories.insert(category);
	}
}

bool XMLConfigDeltaGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::set<std::string>::iterator it = myRemovedNames.find(name);
	bool updateNewOption = true;
	if (it != myRemovedNames.end()) {
		updateNewOption = false;
		myRemovedNames.erase(it);
	}
	std::map<std::string,XMLConfigValue>::iterator jt = myValues.find(name);
	if (jt != myValues.end()) {
		updateNewOption = false;
		if (jt->second.Category == category) {
			jt->second.Value = value;
			return updateNewOption;
		} else {
			myValues.erase(jt);
		}
	}
	std::set<std::string>::iterator kt = myCategories.find(category);
	if (kt == myCategories.end()) {
		kt = myCategories.insert(category).first;
	}
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*kt, value)));
	return updateNewOption;
}

bool XMLConfigDeltaGroup::unsetValue(const std::string &name) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	bool updateNewOption = true;
	if (it != myValues.end()) {
		updateNewOption = false;
		myValues.erase(it);
	}
	myRemovedNames.insert(name);
	return updateNewOption;
}

void XMLConfigDelta::clear() {
	for (std::map<std::string,XMLConfigDeltaGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
	myGroups.clear();
	myChangesCounter = 0;
	myCategories.clear();
	myIsUpToDate = true;
}

XMLConfigDelta::~XMLConfigDelta() {
	clear();
}

XMLConfigDeltaGroup *XMLConfigDelta::getGroup(const std::string &name) {
	std::map<std::string,XMLConfigDeltaGroup*>::const_iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		return it-> second;
	}
	XMLConfigDeltaGroup *group = new XMLConfigDeltaGroup(myCategories);
	myGroups.insert(std::pair<std::string,XMLConfigDeltaGroup*>(name, group));
	return group;
}
