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

#ifndef __ZLWIN32CONFIG_H__
#define __ZLWIN32CONFIG_H__

#include <map>
#include <set>
#include <string>

#include <windows.h>

#include <shared_ptr.h>
#include <ZLRunnable.h>

#include "../../../../core/src/options/ZLConfig.h"

#include "ZLWin32ConfigValue.h"

class ZLWin32ConfigManager : public ZLConfigManager {

public:
	static void createInstance();

public:
	ZLConfig *createConfig() const;

private:
	ZLWin32ConfigManager();
};

class ZLWin32ConfigGroup {

public:
	ZLWin32ConfigGroup(const std::string &groupName, std::set<std::string> &categories);
	void setValue(const std::string &name, const std::string &value, const std::string &category);

private:
	std::string myName;
	std::map<std::string,ZLWin32ConfigValue> myValues;
	std::set<std::string> &myCategories;

friend class ZLWin32Config;
};

class ZLWin32Config : public ZLConfig {

private:
	static const std::string PSEUDO_GROUPNAME_PREFIX;
	static const std::string REAL_GROUPNAME_KEY;

public:
	ZLWin32Config();
	~ZLWin32Config();

	void listOptionNames(const std::string &groupName, std::vector<std::string> &names);
	void listOptionGroups(std::vector<std::string> &groups);
	void removeGroup(const std::string &name);

	const std::string &getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	const std::string &getDefaultValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);

	bool isAutoSavingSupported() const;
	void startAutoSave(int seconds);

private:
	ZLWin32ConfigGroup *getGroup(const std::string &name, bool createUnexisting);

private:
	void load();

private:
	std::string rootKeyName() const;

	void collectSubKeys(std::set<std::string> &keySet, HKEY root);
	void collectValues(std::set<std::string> &valueSet, HKEY key);
	bool registryGetValue(std::string &value, HKEY key, const std::string &name);

	void registryRemoveValue(const std::string &category, const std::string &group, const std::string &valueName);
	void registrySetValue(const std::string &category, const std::string &group, const std::string &valueName, const std::string &value);

private:
	std::map<std::string,ZLWin32ConfigGroup*> myGroups;
	std::set<std::string> myCategories;

	const int myBufferSize;
	char *myBuffer;

	int myPseudoGroupNameNumber;
	std::map<std::string,std::string> myGroupAliases;
};

#endif /* __ZLWIN32CONFIG_H__ */
