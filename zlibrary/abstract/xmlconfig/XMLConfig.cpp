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

#include <stack>

#include "../../abstract/filesystem/ZLFSManager.h"
#include "../../abstract/filesystem/ZLFSDir.h"
#include "../../abstract/filesystem/ZLInputStream.h"

#include "XMLConfig.h"
#include "XMLConfigReader.h"
#include "XMLConfigWriter.h"
#include "AsciiEncoder.h"

const std::string &XMLConfigGroup::getValue(const std::string &name, const std::string &defaultValue) const {
	std::map<std::string,std::string>::const_iterator it = myValues.find(name);
	return (it != myValues.end()) ? it->second : defaultValue;
}

void XMLConfigGroup::unsetValue(const std::string &name) {
	std::map<std::string,std::string>::iterator it = myValues.find(name);
	if (it != myValues.end()) {
		myValues.erase(it);
	}
}

XMLConfig::XMLConfig(const std::string &name) : myName(name) {
	XMLConfigReader(*this).readDocument(ZLFile("~/." + myName + "/config.xml").inputStream());
}

XMLConfig::~XMLConfig() {
	shared_ptr<ZLDir> configDir = ZLFile("~/." + myName).directory(true);

	if (!configDir.isNull()) {
		shared_ptr<ZLOutputStream> stream = ZLFile("~/." + myName + "/config.xml").outputStream();
		if (!stream.isNull() && stream->open()) {
			XMLConfigWriter(*this, *stream).write();
			stream->close();
		}
	} // TODO: show error message
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.begin(); it != myGroups.end(); it++) {
		delete it->second;
	}
}

XMLConfigGroup *XMLConfig::getGroup(const std::string &name, bool createUnexisting) {
	std::map<std::string,XMLConfigGroup*>::const_iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		return it-> second;
	}
	if (createUnexisting) {
		XMLConfigGroup *group = new XMLConfigGroup();
		myGroups.insert(std::pair<std::string,XMLConfigGroup*>(name, group));
		return group;
	}
	return 0;
}

void XMLConfig::removeGroup(const std::string &name) {
	std::map<std::string,XMLConfigGroup*>::iterator it = myGroups.find(name);
	if (it != myGroups.end()) {
		delete it->second;
		myGroups.erase(it);
	}
}
