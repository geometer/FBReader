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

#include <iostream>

#include <abstract/ZLTime.h>

#include "XMLConfig.h"
#include "XMLConfigDelta.h"

const std::string &XMLConfigGroup::getValue(const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,XMLConfigValue>::const_iterator it = myValues.find(name);
	return (it != myValues.end()) ? it->second.Value : defaultValue;
}

bool XMLConfigGroup::setValue(const std::string &name, const std::string &value, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		if (it->second.Category == category) {
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

bool XMLConfigGroup::setCategory(const std::string &name, const std::string &category) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it == myValues.end() || (it->second.Category == category)) {
		return false;
	}
	std::string value = it->second.Value;
	myValues.erase(it);
	std::set<std::string>::iterator jt = myCategories.find(category);
	if (jt == myCategories.end()) {
		jt = myCategories.insert(category).first;
	}
	myValues.insert(std::pair<std::string,XMLConfigValue>(name, XMLConfigValue(*jt, value)));
	return true;
}

bool XMLConfigGroup::unsetValue(const std::string &name) {
	std::map<std::string,XMLConfigValue>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		myValues.erase(it);
		return true;
	}
	return false;
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
	myConfig.saveDelta();
}

XMLConfig::XMLConfig(const std::string &name, const std::string &homeDirectory) : myHomeDirectory(homeDirectory), myName(name), myDelta(0) {
	load();
	myDelta = new XMLConfigDelta(myCategories);
	mySaver = new ConfigSaveTask(*this);
	ZLTimeManager::instance().addTask(mySaver, 10000);
}

XMLConfig::~XMLConfig() {
	ZLTimeManager::instance().removeTask(mySaver);
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
		if (myDelta != 0) {
			const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
			for (std::map<std::string,XMLConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
				myDelta->unsetValue(name, jt->first);
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
	if ((configGroup != 0) && configGroup->unsetValue(name) && (myDelta != 0)) {
		myDelta->unsetValue(group, name);
	}
}
