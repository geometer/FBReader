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

#ifndef __XMLCONFIGDELTA_H__
#define __XMLCONFIGDELTA_H__

#include <map>
#include <set>
#include <string>

#include "XMLConfigValue.h"

class XMLConfigDeltaGroup {

public:
	XMLConfigDeltaGroup(std::set<std::string> &categories);
	bool setValue(const std::string &name, const std::string &value, const std::string &category);
	bool unsetValue(const std::string &name);

private:
	std::map<std::string,XMLConfigValue> myValues;
	std::set<std::string> myRemovedNames;
	std::set<std::string> &myCategories;

friend class XMLConfigDeltaWriter;
};

class XMLConfigDelta {

public:
	XMLConfigDelta();
	~XMLConfigDelta();
	XMLConfigDeltaGroup *getGroup(const std::string &name);
	void clear();

private:
	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);
	void addCategory(const std::string &category);

	std::map<std::string,XMLConfigDeltaGroup*> myGroups;
	std::set<std::string> myCategories;

	int myChangesCounter;
	bool myIsUpToDate;

friend class XMLConfigReader;
friend class XMLConfigDeltaWriter;
friend class XMLConfig;
};

inline XMLConfigDeltaGroup::XMLConfigDeltaGroup(std::set<std::string> &categories) : myCategories(categories) {
}

inline XMLConfigDelta::XMLConfigDelta() : myChangesCounter(0), myIsUpToDate(true) {
}

inline void XMLConfigDelta::setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category) {
	if (getGroup(group)->setValue(name, value, category)) {
		++myChangesCounter;
	}
	myIsUpToDate = false;
}

inline void XMLConfigDelta::unsetValue(const std::string &group, const std::string &name) {
	if (getGroup(group)->unsetValue(name)) {
		++myChangesCounter;
	}
	myIsUpToDate = false;
}

#endif /* __XMLCONFIGDELTA_H__ */
