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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include <algorithm>

#include "ZLOptions.h"
#include "ZLConfig.h"

ZLConfig *ZLOption::ourConfig = 0;

void ZLOption::unsetConfigValue() const {
	ourConfig->unsetValue(myGroupName, myOptionName);
}

void ZLOption::setConfigValue(const std::string &value) const {
	if (myCategory != ZLCategoryKey::EMPTY) { 
		ourConfig->setValue(myGroupName, myOptionName, value, myCategory.Name);
	}
}

static const std::string EMPTY_STRING;

const std::string &ZLOption::getConfigValue(const std::string &defaultValue) const {
	return ourConfig->getValue(myGroupName, myOptionName, defaultValue);
}

const std::string &ZLOption::getConfigValue() const {
	return getConfigValue(EMPTY_STRING);
}

const std::string &ZLOption::getDefaultConfigValue(const std::string &defaultValue) const {
	return ourConfig->getDefaultValue(myGroupName, myOptionName, defaultValue);
}

const std::string &ZLOption::getDefaultConfigValue() const {
	return getDefaultConfigValue(EMPTY_STRING);
}

void ZLOption::createInstance() {
	ourConfig = ZLConfigManager::instance().createConfig();
}

void ZLOption::deleteInstance() {
	delete ourConfig;
	ourConfig = 0;
}

static std::string booleanToString(bool value) {
	return value ? "true" : "false";
}
static bool stringToBoolean(const std::string &value, bool defaultValue) {
	return (value.empty()) ? defaultValue : (value == "true");
}

static std::string integerToString(long value) {
	char buf[100];
	sprintf(buf, "%ld", value);
	return buf;
}
static long stringToInteger(const std::string &value, long defaultValue) {
	return (!value.empty()) ? atoi(value.c_str()) : defaultValue;
}
static long stringToInteger(const std::string &value, long defaultValue, long minValue, long maxValue) {
	return std::max(minValue, std::min(maxValue, stringToInteger(value, defaultValue)));
}

static std::string doubleToString(double value) {
	char buf[100];
	setlocale(LC_NUMERIC, "C");
	sprintf(buf, "%f", value);
	return buf;
}
static double stringToDouble(const std::string &value, double defaultValue) {
	if (!value.empty()) {
		setlocale(LC_NUMERIC, "C");
		return atof(value.c_str());
	} else {
		return defaultValue;
	}
}

const std::string ZLOption::PLATFORM_GROUP = "PlatformOptions";
const std::string ZLOption::FULL_KEYBOARD_CONTROL = "FullKeyboardControlSupported";
const std::string ZLOption::KEYBOARD_PRESENTED = "KeyboardPresented";
const std::string ZLOption::MOUSE_PRESENTED = "MousePresented";
const std::string ZLOption::TOUCHSCREEN_PRESENTED = "TouchScreenPresented";
const std::string ZLOption::FINGER_TAP_DETECTABLE = "FingerTapDetectable";

void ZLOption::clearGroup(const std::string &groupName) {
	ourConfig->removeGroup(groupName);
}

void ZLOption::listOptionNames(const std::string &groupName, std::vector<std::string> &names) {
	ourConfig->listOptionNames(groupName, names);
}

void ZLOption::listOptionGroups(std::vector<std::string> &groups) {
	ourConfig->listOptionGroups(groups);
}

void ZLOption::startAutoSave(int seconds) {
	ourConfig->startAutoSave(seconds);
}

bool ZLOption::isAutoSavingSupported() {
	return ourConfig->isAutoSavingSupported();
}

ZLOption::ZLOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName) : myCategory(category), myGroupName(groupName), myOptionName(optionName), myIsSynchronized(false) {
}

ZLOption::~ZLOption() {
}

ZLSimpleOption::ZLSimpleOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName) : ZLOption(category, groupName, optionName) {
}

ZLBooleanOption::ZLBooleanOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, bool defaultValue) : ZLSimpleOption(category, groupName, optionName), myDefaultValue(stringToBoolean(getDefaultConfigValue(), defaultValue)) {
}

ZLSimpleOption::Type ZLBooleanOption::type() const {
	return TYPE_BOOLEAN;
}

bool ZLBooleanOption::value() const {
	if (!myIsSynchronized) {
		myValue = stringToBoolean(getConfigValue(), myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLBooleanOption::setValue(bool value) {
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(booleanToString(myValue));
	}
}

ZLBoolean3Option::ZLBoolean3Option(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, ZLBoolean3 defaultValue) : ZLSimpleOption(category, groupName, optionName), myDefaultValue((ZLBoolean3)stringToInteger(getDefaultConfigValue(), defaultValue)) {
}

ZLSimpleOption::Type ZLBoolean3Option::type() const {
	return TYPE_BOOLEAN3;
}

ZLBoolean3 ZLBoolean3Option::value() const {
	if (!myIsSynchronized) {
		myValue = (ZLBoolean3)stringToInteger(getConfigValue(), myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLBoolean3Option::setValue(ZLBoolean3 value) {
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(integerToString(myValue));
	}
}

ZLColorOption::ZLColorOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, ZLColor defaultValue) : ZLOption(category, groupName, optionName), myDefaultIntValue(stringToInteger(getDefaultConfigValue(), defaultValue.intValue())) {
}

ZLColor ZLColorOption::value() const {
	if (!myIsSynchronized) {
		myIntValue = stringToInteger(getConfigValue(), myDefaultIntValue);
		myIsSynchronized = true;
	}
	return ZLColor(myIntValue);
}

void ZLColorOption::setValue(ZLColor value) {
	if (myIsSynchronized && (myIntValue == value.intValue())) {
		return;
	}
	myIntValue = value.intValue();
	myIsSynchronized = true;
	if (myIntValue == myDefaultIntValue) {
		unsetConfigValue();
	} else {
		setConfigValue(integerToString(myIntValue));
	}
}

ZLIntegerOption::ZLIntegerOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, long defaultValue) : ZLOption(category, groupName, optionName), myDefaultValue(stringToInteger(getDefaultConfigValue(), defaultValue)) {
}

long ZLIntegerOption::value() const {
	if (!myIsSynchronized) {
		myValue = stringToInteger(getConfigValue(), myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLIntegerOption::setValue(long value) {
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(integerToString(myValue));
	}
}

ZLIntegerRangeOption::ZLIntegerRangeOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, long minValue, long maxValue, long defaultValue) : ZLOption(category, groupName, optionName), myMinValue(minValue), myMaxValue(maxValue), myDefaultValue(stringToInteger(getDefaultConfigValue(), defaultValue, minValue, maxValue)) {
}

long ZLIntegerRangeOption::value() const {
	if (!myIsSynchronized) {
		myValue = stringToInteger(getConfigValue(), myDefaultValue, myMinValue, myMaxValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLIntegerRangeOption::setValue(long value) {
	value = std::max(std::min(myMaxValue, value), myMinValue);
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(integerToString(myValue));
	}
}

long ZLIntegerRangeOption::minValue() const {
	return myMinValue;
}

long ZLIntegerRangeOption::maxValue() const {
	return myMaxValue;
}

ZLDoubleOption::ZLDoubleOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, double defaultValue) : ZLOption(category, groupName, optionName), myDefaultValue(stringToDouble(getDefaultConfigValue(), defaultValue)) {
}

double ZLDoubleOption::value() const {
	if (!myIsSynchronized) {
		myValue = stringToDouble(getConfigValue(), myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLDoubleOption::setValue(double value) {
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(doubleToString(myValue));
	}
}

ZLStringOption::ZLStringOption(const ZLCategoryKey &category, const std::string &groupName, const std::string &optionName, const std::string &defaultValue) : ZLSimpleOption(category, groupName, optionName), myDefaultValue(getDefaultConfigValue(defaultValue)) {
}

ZLSimpleOption::Type ZLStringOption::type() const {
	return TYPE_STRING;
}

const std::string &ZLStringOption::value() const {
	if (!myIsSynchronized) {
		myValue = getConfigValue(myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLStringOption::setValue(const std::string &value) {
	if (myIsSynchronized && (myValue == value)) {
		return;
	}
	myValue = value;
	myIsSynchronized = true;
	if (myValue == myDefaultValue) {
		unsetConfigValue();
	} else {
		setConfigValue(myValue);
	}
}
