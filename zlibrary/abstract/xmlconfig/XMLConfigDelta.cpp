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

#include "XMLConfigDelta.h"

void XMLConfigDeltaGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		if (it->second.Category == category) {
			it->second.Value = value;
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

void XMLConfigDelta::clear() {
	for (std::map<std::string,XMLConfigDeltaGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
	myGroups.clear();
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
