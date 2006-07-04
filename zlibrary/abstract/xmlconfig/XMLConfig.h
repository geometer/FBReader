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

#ifndef __XMLCONFIG_H__
#define __XMLCONFIG_H__

#include <map>
#include <set>
#include <string>

#include "XMLConfigValue.h"

class XMLConfigGroup {

public:
	XMLConfigGroup(std::set<std::string> &categories) : myCategories(categories) {}
	const std::string &getValue(const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &name);
	void setCategory(const std::string &name, const std::string &category);

private:
	std::map<std::string,XMLConfigValue> myValues;
	std::set<std::string> &myCategories;

friend class XMLConfigWriter;
friend class XMLConfig;
};

class XMLConfig {

public:
	XMLConfig(const std::string &name, const std::string &homeDirectory);
	~XMLConfig();

	void removeGroup(const std::string &name);

	const std::string &getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);
	void setCategory(const std::string &group, const std::string &name, const std::string &category);

private:
	XMLConfigGroup *getGroup(const std::string &name) const;
	XMLConfigGroup *getGroup(const std::string &name, bool createUnexisting);

private:
	void load();
	void saveAll();
	void saveDelta();

private:
	std::string myHomeDirectory;
	std::string myName;
	std::map<std::string,XMLConfigGroup*> myGroups;
	std::set<std::string> myCategories;
	class XMLConfigDelta *myDelta;

friend class XMLConfigWriter;
friend class XMLConfigReader;
};

inline const std::string &XMLConfig::getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const {
	XMLConfigGroup *configGroup = getGroup(group);
	return (configGroup != 0) ? configGroup->getValue(name, defaultValue) : defaultValue;
}

inline void XMLConfig::setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category) {
	getGroup(group, true)->setValue(name, value, category);
}

inline void XMLConfig::unsetValue(const std::string &group, const std::string &name) {
	XMLConfigGroup *configGroup = getGroup(group, false);
	if (configGroup != 0) {
		configGroup->unsetValue(name);
	}
}

inline void XMLConfig::setCategory(const std::string &group, const std::string &name, const std::string &category) {
	getGroup(group, true)->setCategory(name, category);
}

#endif /* __XMLCONFIG_H__ */
