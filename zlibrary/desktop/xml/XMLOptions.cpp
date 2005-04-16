/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>

#include "XMLOptions.h"
#include "XMLConfig.h"

XMLOptions::XMLOptions(const std::string &name) {
	myConfig = new XMLConfig(name);
}

XMLOptions::~XMLOptions() {
	delete myConfig;
}

void XMLOptions::setGroup(const std::string &name){
	myGroupName = name;
}

void XMLOptions::clearGroup() {
	myConfig->removeGroup(myGroupName);
}

void XMLOptions::unsetValue(const std::string &name) {
	XMLConfigGroup *group = myConfig->getGroup(myGroupName, false);
	if (group != 0) {
		group->unsetValue(name);
	}
}

bool XMLOptions::booleanValue(const std::string &name, bool defaultValue) {
	return stringValue(name, defaultValue ? "true" : "false") == "true";
}

void XMLOptions::setValue(const std::string &name, bool value) {
	setValue(name, std::string(value ? "true" : "false"));
}

long XMLOptions::integerValue(const std::string &name, long defaultValue) {
	std::string value = stringValue(name, std::string());
	return (!value.empty()) ? atoi(value.c_str()) : defaultValue;
}

void XMLOptions::setValue(const std::string &name, long value) {
	char buf[100];
	sprintf(buf, "%d", value);
	setValue(name, std::string(buf));
}

double XMLOptions::doubleValue(const std::string &name, double defaultValue) {
	std::string value = stringValue(name, std::string());
	return (!value.empty()) ? atof(value.c_str()) : defaultValue;
}

void XMLOptions::setValue(const std::string &name, double value) {
	char buf[100];
	sprintf(buf, "%f", value);
	setValue(name, std::string(buf));
}

std::string XMLOptions::stringValue(const std::string &name, const std::string &defaultValue) {
	XMLConfigGroup *group = myConfig->getGroup(myGroupName, false);
	return (group != 0) ? group->getValue(name, defaultValue) : defaultValue;
}

void XMLOptions::setValue(const std::string &name, const std::string &value) {
	myConfig->getGroup(myGroupName, true)->setValue(name, value);
}
