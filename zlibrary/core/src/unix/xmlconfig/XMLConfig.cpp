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

#include "XMLConfig.h"
#include "XMLConfigDelta.h"

void XMLConfigManager::createInstance() {
	ourInstance = new XMLConfigManager();
}

XMLConfigManager::XMLConfigManager() {
}

ZLConfig *XMLConfigManager::createConfig() const {
	ZLConfig *config = new XMLConfig();
	ourIsInitialised = true;
	return config;
}

const std::string &XMLConfigGroup::getValue(const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,XMLConfigValue>::const_iterator it = myValues.find(name);
	return (it != myValues.end()) ? it->second.Value : defaultValue;
}

bool XMLConfigGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		if ((category == it->second.Category) || (category == XMLConfig::UNKNOWN_CATEGORY)) {
			if (it->second.Value != value) {
				it->second.Value = value;
				return true;
			} else {
				return false;
			}
		} else {
			myValues.erase(it);
		}
	}
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
	return true;
}

void XMLConfigGroup::unsetValue(const std::string &name) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		myValues.erase(it);
	}
}

class ConfigSaveTask : public ZLRunnable {

public:
	ConfigSaveTask(XMLConfig &config);

private:
	void run();

private:
	XMLConfig &myConfig;
};

ConfigSaveTask::ConfigSaveTask(XMLConfig &config) : myConfig(config) {
}

void ConfigSaveTask::run() {
	if (myConfig.changesCounter() >= 500) {
		myConfig.saveAll();
	} else {
		myConfig.saveDelta();
	}
}

XMLConfig::XMLConfig() : myDelta(0) {
	load();
	mySaver = new ConfigSaveTask(*this);
}

XMLConfig::~XMLConfig() {
	ZLTimeManager::instance().removeTask(mySaver);
	saveAll();
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		delete it->second;
	}
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myDefaultGroups.begin(); it != myDefaultGroups.end(); ++it) {
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

XMLConfigGroup *XMLConfig::getDefaultGroup(const std::string &name) const {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myDefaultGroups.find(name);
	return (it != myDefaultGroups.end()) ? it-> second : 0;
}

void XMLConfig::listOptionNames(const std::string &groupName, std::vector<std::string> &names) {
	std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.find(groupName);
	if (it == myGroups.end()) {
		return;
	}

	const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
	for (std::map<std::string,XMLConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
		names.push_back(jt->first);
	}
}

void XMLConfig::listOptionGroups(std::vector<std::string> &groups) {
	for (std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.begin(); it != myGroups.end(); ++it) {
		groups.push_back(it->first);
	}
}

void XMLConfig::removeGroup(const std::string &name) {
	std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		if (myDelta != 0) {
			const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
			for (std::map<std::string,XMLConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
				myDelta->unsetValue(name, jt->first);
				myDelta->addCategory(jt->second.Category);
			}
		}
		delete it->second;
		myGroups.erase(it);
	}
}

void XMLConfig::setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category) {
	if (getGroup(group, true)->setValue(name, value, category) && (myDelta != 0)) {
		myDelta->setValue(group, name, value, category);
	}
}

void XMLConfig::unsetValue(const std::string &group, const std::string &name) {
	XMLConfigGroup *configGroup = getGroup(group, false);
	if (configGroup == 0) {
		return;
	}
	std::map<std::string,XMLConfigValue>::iterator it = configGroup->myValues.find(name);
	if (it != configGroup->myValues.end()) {
		if (myDelta != 0) {
			myDelta->addCategory(it->second.Category);
			myDelta->unsetValue(group, name);
		}
		configGroup->myValues.erase(it);
	}
}

int XMLConfig::changesCounter() const {
	return (myDelta != 0) ? myDelta->myChangesCounter : 0;
}

bool XMLConfig::isAutoSavingSupported() const {
	return true;
}

void XMLConfig::startAutoSave(int seconds) {
	ZLTimeManager::instance().addTask(mySaver, 1000 * seconds);
}
