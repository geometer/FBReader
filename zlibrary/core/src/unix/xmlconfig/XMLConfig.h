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

#ifndef __XMLCONFIG_H__
#define __XMLCONFIG_H__

#include <map>
#include <set>
#include <string>

#include <shared_ptr.h>
#include <ZLRunnable.h>

#include "../../options/ZLConfig.h"

#include "XMLConfigValue.h"

class XMLConfigManager : public ZLConfigManager {

public:
	static void createInstance();

public:
	ZLConfig *createConfig() const;

private:
	XMLConfigManager();
};

class XMLConfigGroup {

public:
	XMLConfigGroup(std::set<std::string> &categories) : myCategories(categories) {}
	const std::string &getValue(const std::string &name, const std::string &defaultValue) const;
	bool setValue(const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &name);

private:
	std::map<std::string,XMLConfigValue> myValues;
	std::set<std::string> &myCategories;

friend class XMLConfigWriter;
friend class XMLConfig;
};

class XMLConfig : public ZLConfig {

public:
	static const std::string UNKNOWN_CATEGORY;

public:
	XMLConfig();
	~XMLConfig();

	void listOptionNames(const std::string &groupName, std::vector<std::string> &names);
	void listOptionGroups(std::vector<std::string> &groups);
	void removeGroup(const std::string &name);

	const std::string &getDefaultValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	const std::string &getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);

	bool isAutoSavingSupported() const;
	void startAutoSave(int seconds);

private:
	XMLConfigGroup *getGroup(const std::string &name) const;
	XMLConfigGroup *getDefaultGroup(const std::string &name) const;
	XMLConfigGroup *getGroup(const std::string &name, bool createUnexisting);

private:
	void load();
	void saveAll();
	void saveDelta();
	std::string configDirName() const;

	int changesCounter() const;

private:
	std::map<std::string,XMLConfigGroup*> myDefaultGroups;
	std::map<std::string,XMLConfigGroup*> myGroups;
	std::set<std::string> myCategories;
	class XMLConfigDelta *myDelta;

	shared_ptr<ZLRunnable> mySaver;

friend class XMLConfigWriter;
friend class XMLConfigReader;
friend class ConfigSaveTask;
};

inline const std::string &XMLConfig::getValue(const std::string &group, const std::string &name, const std::string &defaultValue) const {
	XMLConfigGroup *configGroup = getGroup(group);
	return (configGroup != 0) ? configGroup->getValue(name, defaultValue) : defaultValue;
}

inline const std::string &XMLConfig::getDefaultValue(const std::string &group, const std::string &name, const std::string &defaultValue) const {
	XMLConfigGroup *configGroup = getDefaultGroup(group);
	return (configGroup != 0) ? configGroup->getValue(name, defaultValue) : defaultValue;
}

#endif /* __XMLCONFIG_H__ */
