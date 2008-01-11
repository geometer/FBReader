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
#include "ZLOptions_internal.h"
#include "ZLConfig.h"

ZLOptions *ZLOptions::ourInstance = 0;

ZLOptions &ZLOptions::instance() {
	return *ourInstance;
}

void ZLOptions::deleteInstance() {
	delete ourInstance;
}

ZLOptions::ZLOptions() {
	myConfig = ZLConfigManager::instance().createConfig();
}

ZLOptions::~ZLOptions() {
	delete myConfig;
}

void ZLOptions::setGroup(const std::string &name){
	myGroupName = name;
}

void ZLOptions::clearGroup() {
	myConfig->removeGroup(myGroupName);
}

void ZLOptions::unsetValue(const std::string &name) {
	myConfig->unsetValue(myGroupName, name);
}

bool ZLOptions::booleanValue(const std::string &name, bool defaultValue) {
	return stringValue(name, defaultValue ? "true" : "false") == "true";
}

void ZLOptions::setValue(const std::string &name, bool value, const ZLCategoryKey &category) {
	setValue(name, std::string(value ? "true" : "false"), category);
}

long ZLOptions::integerValue(const std::string &name, long defaultValue) {
	std::string value = stringValue(name, std::string());
	return (!value.empty()) ? atoi(value.c_str()) : defaultValue;
}

void ZLOptions::setValue(const std::string &name, long value, const ZLCategoryKey &category) {
	char buf[100];
	sprintf(buf, "%ld", value);
	setValue(name, std::string(buf), category);
}

double ZLOptions::doubleValue(const std::string &name, double defaultValue) {
	std::string value = stringValue(name, std::string());
	setlocale(LC_NUMERIC, "C");
	return (!value.empty()) ? atof(value.c_str()) : defaultValue;
}

void ZLOptions::setValue(const std::string &name, double value, const ZLCategoryKey &category) {
	char buf[100];
	setlocale(LC_NUMERIC, "C");
	sprintf(buf, "%f", value);
	setValue(name, std::string(buf), category);
}

std::string ZLOptions::stringValue(const std::string &name, const std::string &defaultValue) {
	return myConfig->getValue(myGroupName, name, defaultValue);
}

void ZLOptions::setValue(const std::string &name, const std::string &value, const ZLCategoryKey &category) {
	if (category != ZLCategoryKey::EMPTY) { 
		myConfig->setValue(myGroupName, name, value, category.Name);
	}
}

bool ZLOptions::isAutoSavingSupported() const {
	return myConfig->isAutoSavingSupported();
}

void ZLOptions::startAutoSave(int seconds) {
	myConfig->startAutoSave(seconds);
}

const std::string ZLOption::PLATFORM_GROUP = "PlatformOptions";
const std::string ZLOption::FULL_KEYBOARD_CONTROL = "FullKeyboardControlSupported";
const std::string ZLOption::KEYBOARD_PRESENTED = "KeyboardPresented";
const std::string ZLOption::MOUSE_PRESENTED = "MousePresented";
const std::string ZLOption::TOUCHSCREEN_PRESENTED = "TouchScreenPresented";
const std::string ZLOption::FINGER_TAP_DETECTABLE = "FingerTapDetectable";

void ZLOption::clearGroup(const std::string &group) {
	ZLOptions::instance().setGroup(group);
	ZLOptions::instance().clearGroup();
}

void ZLOption::startAutoSave(int seconds) {
	ZLOptions::instance().startAutoSave(seconds);
}

bool ZLOption::isAutoSavingSupported() {
	return ZLOptions::instance().isAutoSavingSupported();
}

ZLOption::ZLOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName) : myCategory(category), myGroup(group), myOptionName(optionName), myIsSynchronized(false) {
}

ZLOption::~ZLOption() {
}

ZLSimpleOption::ZLSimpleOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName) : ZLOption(category, group, optionName) {
}

ZLBooleanOption::ZLBooleanOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, bool defaultValue) : ZLSimpleOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLBooleanOption::~ZLBooleanOption() {
}

ZLSimpleOption::Type ZLBooleanOption::type() const {
	return TYPE_BOOLEAN;
}

bool ZLBooleanOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = options.booleanValue(myOptionName, myDefaultValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myValue, myCategory);
	}
}

ZLBoolean3Option::ZLBoolean3Option(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, ZLBoolean3 defaultValue) : ZLSimpleOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLBoolean3Option::~ZLBoolean3Option() {
}

ZLSimpleOption::Type ZLBoolean3Option::type() const {
	return TYPE_BOOLEAN3;
}

ZLBoolean3 ZLBoolean3Option::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = (ZLBoolean3)options.integerValue(myOptionName, myDefaultValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, (long)myValue, myCategory);
	}
}

ZLColorOption::ZLColorOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, ZLColor defaultValue) : ZLOption(category, group, optionName), myDefaultIntValue(defaultValue.intValue()) {
}

ZLColorOption::~ZLColorOption() {
}

ZLColor ZLColorOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myIntValue = options.integerValue(myOptionName, myDefaultIntValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myIntValue == myDefaultIntValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myIntValue, myCategory);
	}
}

ZLIntegerOption::ZLIntegerOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, long defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLIntegerOption::~ZLIntegerOption() {
}

long ZLIntegerOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = options.integerValue(myOptionName, myDefaultValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myValue, myCategory);
	}
}

ZLIntegerRangeOption::ZLIntegerRangeOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, long minValue, long maxValue, long defaultValue) : ZLOption(category, group, optionName), myMinValue(minValue), myMaxValue(maxValue), myDefaultValue(defaultValue) {
}

ZLIntegerRangeOption::~ZLIntegerRangeOption() {
}

long ZLIntegerRangeOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = options.integerValue(myOptionName, myDefaultValue);
		myValue = std::max(std::min(myMaxValue, myValue), myMinValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myValue, myCategory);
	}
}

long ZLIntegerRangeOption::minValue() const {
	return myMinValue;
}

long ZLIntegerRangeOption::maxValue() const {
	return myMaxValue;
}

ZLDoubleOption::ZLDoubleOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, double defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLDoubleOption::~ZLDoubleOption() {
}

double ZLDoubleOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = options.doubleValue(myOptionName, myDefaultValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myValue, myCategory);
	}
}

ZLStringOption::ZLStringOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, const std::string &defaultValue) : ZLSimpleOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLStringOption::~ZLStringOption() {
}

ZLSimpleOption::Type ZLStringOption::type() const {
	return TYPE_STRING;
}

const std::string &ZLStringOption::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = options.stringValue(myOptionName, myDefaultValue);
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
	ZLOptions::instance().setGroup(myGroup);
	if (myValue == myDefaultValue) {
		ZLOptions::instance().unsetValue(myOptionName);
	} else {
		ZLOptions::instance().setValue(myOptionName, myValue, myCategory);
	}
}
