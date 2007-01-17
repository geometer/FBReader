/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include "ZLOptions.h"
#include "ZLOptions_internal.h"

ZLOptions *ZLOptions::ourInstance = 0;

ZLOptions::ZLOptions() {
}

ZLOptions::~ZLOptions() {
}

ZLOptions &ZLOptions::instance() {
	return *ourInstance;
}

void ZLOptions::deleteInstance() {
	delete ourInstance;
}

const std::string ZLOption::LOOK_AND_FEEL_CATEGORY = "ui";
const std::string ZLOption::CONFIG_CATEGORY = "options";
const std::string ZLOption::STATE_CATEGORY = "state";

void ZLOption::clearGroup(const std::string &group) {
	ZLOptions::instance().setGroup(group);
	ZLOptions::instance().clearGroup();
}

void ZLOption::startAutoSave(int seconds) {
	ZLOptions::instance().startAutoSave(seconds);
}

ZLOption::ZLOption(const std::string &category, const std::string &group, const std::string &optionName) : myCategory(category), myGroup(group), myOptionName(optionName), myIsSynchronized(false) {
}

ZLOption::~ZLOption() {
}

ZLBooleanOption::ZLBooleanOption(const std::string &category, const std::string &group, const std::string &optionName, bool defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLBooleanOption::~ZLBooleanOption() {
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

ZLBoolean3Option::ZLBoolean3Option(const std::string &category, const std::string &group, const std::string &optionName, Boolean3 defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLBoolean3Option::~ZLBoolean3Option() {
}

Boolean3 ZLBoolean3Option::value() const {
	if (!myIsSynchronized) {
		ZLOptions &options = ZLOptions::instance();
		options.setGroup(myGroup);
		myValue = (Boolean3)options.integerValue(myOptionName, myDefaultValue);
		myIsSynchronized = true;
	}
	return myValue;
}

void ZLBoolean3Option::setValue(Boolean3 value) {
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

ZLColorOption::ZLColorOption(const std::string &category, const std::string &group, const std::string &optionName, ZLColor defaultValue) : ZLOption(category, group, optionName), myDefaultIntValue(defaultValue.intValue()) {
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

ZLIntegerOption::ZLIntegerOption(const std::string &category, const std::string &group, const std::string &optionName, long defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
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

ZLIntegerRangeOption::ZLIntegerRangeOption(const std::string &category, const std::string &group, const std::string &optionName, long minValue, long maxValue, long defaultValue) : ZLOption(category, group, optionName), myMinValue(minValue), myMaxValue(maxValue), myDefaultValue(defaultValue) {
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

ZLDoubleOption::ZLDoubleOption(const std::string &category, const std::string &group, const std::string &optionName, double defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
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

ZLStringOption::ZLStringOption(const std::string &category, const std::string &group, const std::string &optionName, const std::string &defaultValue) : ZLOption(category, group, optionName), myDefaultValue(defaultValue) {
}

ZLStringOption::~ZLStringOption() {
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
