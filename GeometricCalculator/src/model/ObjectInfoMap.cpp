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
