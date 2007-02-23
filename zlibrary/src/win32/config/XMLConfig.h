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

#ifndef __XMLCONFIG_H__
#define __XMLCONFIG_H__

#include <map>
#include <set>
#include <string>

#include <windows.h>

#include <shared_ptr.h>
#include <ZLRunnable.h>

#include "XMLConfigValue.h"

class XMLConfigGroup {

public:
	XMLConfigGroup(const std::string &groupName, std::set<std::string> &categories);
	void setValue(const std::string &name, const std::string &value, const std::string &category);

private:
	std::string myName;
	std::map<std::string,XMLConfigValue> myValues;
	std::set<std::string> &myCategories;

friend class XMLConfig;
};

class XMLConfig {

public:
	XMLConfig();
	~XMLConfig();

	void removeGroup(const std::string &name);

	const std::string &getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);

private:
	XMLConfigGroup *getGroup(const std::string &name, bool createUnexisting);

private:
	void load();

private:
	std::string rootKeyName() const;

	void collectSubKeys(std::set<std::string> &keySet, HKEY root);
	void collectValues(std::set<std::string> &valueSet, HKEY key);
	bool getValue(std::string &value, HKEY key, const std::string &name);

	void removeValue(const std::string &keyName, const std::string &valueName);
	void setValue(const std::string &keyName, const std::string &valueName, const std::string &value);

private:
	std::map<std::string,XMLConfigGroup*> myGroups;
	std::set<std::string> myCategories;

	const int myBufferSize;
	char *myBuffer;
};

#endif /* __XMLCONFIG_H__ */
