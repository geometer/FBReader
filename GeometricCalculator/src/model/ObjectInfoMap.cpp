/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <locale.h>

#include <ZLStringUtil.h>

#include "Object.h"

ObjectInfoMap::ObjectInfoMap(ObjectEnumerator &enumerator) : myEnumerator(enumerator) {
}

void ObjectInfoMap::addAttribute(const std::string &name, const std::string &value) {
	myAttributes[name] = value;
}

void ObjectInfoMap::addAttribute(const std::string &name, int value) {
	std::string strValue;
	ZLStringUtil::appendNumber(strValue, value);
	addAttribute(name, strValue);
}

void ObjectInfoMap::addAttribute(const std::string &name, double value) {
	std::string str;
	if (value < 0) {
		str += "-";
		value = -value;
	}
	ZLStringUtil::appendNumber(str, (int)value);
	str += ".";
	ZLStringUtil::appendNumber(str, (int)(1000 * (value - (int)value) + 0.5));
	addAttribute(name, str);
}

void ObjectInfoMap::addAttribute(const std::string &name, bool value) {
	std::string strValue = value ? "true" : "false";
	addAttribute(name, strValue);
}

void ObjectInfoMap::addAttribute(const std::string &name, ObjectPtr value) {
	int id = myEnumerator.id(value);
	if (id != -1) {
		addAttribute(name, id);
	}
}

const std::map<std::string,std::string> &ObjectInfoMap::data() const {
	return myAttributes;
}

const std::string &ObjectInfoMap::getStringValue(const std::string &name) const {
	std::map<std::string,std::string>::const_iterator it = myAttributes.find(name);
	if (it != myAttributes.end()) {
		return it->second;
	}
	static const std::string EMPTY;
	return EMPTY;
}

bool ObjectInfoMap::stringValueEqualsTo(const std::string &name, const std::string &value) const {
	return getStringValue(name) == value;
}

int ObjectInfoMap::getIntValue(const std::string &name) const {
	const std::string &stringValue = getStringValue(name);
	return stringValue.empty() ? 0 : atoi(stringValue.c_str());
}

double ObjectInfoMap::getDoubleValue(const std::string &name) const {
	const std::string &stringValue = getStringValue(name);
	setlocale(LC_NUMERIC, "C");
	return stringValue.empty() ? 0.0 : atof(stringValue.c_str());
}

bool ObjectInfoMap::getBooleanValue(const std::string &name) const {
  return stringValueEqualsTo(name, "true");
}

ObjectPtr ObjectInfoMap::getObjectValue(const std::string &name) const {
	return myEnumerator.object(getIntValue(name));
}
