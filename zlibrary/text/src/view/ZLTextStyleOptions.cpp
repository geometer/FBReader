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

#include <algorithm>

#include <ZLPaintContext.h>

#include "ZLTextStyleOptions.h"
#include "ZLTextStyle.h"

static const std::string KEY_UNCHANGED = "unchanged";
static const std::string KEY_LEFT = "left";
static const std::string KEY_RIGHT = "right";
static const std::string KEY_CENTER = "center";
static const std::string KEY_JUSTIFY = "justify";

std::vector<std::string> ZLTextFontFamilyWithBaseOptionEntry::ourAllFamilies;

std::vector<std::string> ZLTextLineSpacingOptionEntry::ourAllValues;
std::vector<std::string> ZLTextLineSpacingOptionEntry::ourAllValuesPlusBase;

std::vector<std::string> ZLTextLineSpaceOptionEntry::ourAllValues;
std::vector<std::string> ZLTextLineSpaceOptionEntry::ourAllValuesPlusBase;

ZLTextFontFamilyWithBaseOptionEntry::ZLTextFontFamilyWithBaseOptionEntry(ZLStringOption &option, const ZLResource &resource, const ZLPaintContext &context) : ZLFontFamilyOptionEntry(option, context), myResource(resource) {
}

const std::vector<std::string> &ZLTextFontFamilyWithBaseOptionEntry::values() const {
	if (ourAllFamilies.empty()) {
		const std::vector<std::string> &families = ZLFontFamilyOptionEntry::values();
		ourAllFamilies.reserve(families.size() + 1);
		ourAllFamilies.push_back(myResource[KEY_UNCHANGED].value());
		ourAllFamilies.insert(ourAllFamilies.end(), families.begin(), families.end());
	}
	return ourAllFamilies;
}

const std::string &ZLTextFontFamilyWithBaseOptionEntry::initialValue() const {
	const std::string &value = ZLFontFamilyOptionEntry::initialValue();
	return value.empty() ? values()[0] : value;
}

void ZLTextFontFamilyWithBaseOptionEntry::onAccept(const std::string &value) {
	ZLFontFamilyOptionEntry::onAccept((value == values()[0]) ? std::string() : value);
}

ZLTextLineSpacingOptionEntry::ZLTextLineSpacingOptionEntry(ZLDoubleOption &option, const ZLResource &resource, bool allowBase) : myResource(resource), myOption(option), myAllowBase(allowBase) {
	if (ourAllValuesPlusBase.empty()) {
		for (int i = 5; i <= 20; ++i) {
			ourAllValues.push_back(std::string() + (char)(i / 10 + '0') + '.' + (char)(i % 10 + '0'));
		}
		ourAllValuesPlusBase.push_back(myResource[KEY_UNCHANGED].value());
		ourAllValuesPlusBase.insert(ourAllValuesPlusBase.end(), ourAllValues.begin(), ourAllValues.end());
	}
}

ZLTextLineSpacingOptionEntry::~ZLTextLineSpacingOptionEntry() {
}

const std::vector<std::string> &ZLTextLineSpacingOptionEntry::values() const { return myAllowBase ? ourAllValuesPlusBase : ourAllValues; }

const std::string &ZLTextLineSpacingOptionEntry::initialValue() const {
	int value = (int)(10 * myOption.value() + 0.5);
	if (value == 0) {
		return ourAllValuesPlusBase[0];
	}
	for (int i = 5; i < 20; ++i) {
		if (value <= i) {
			return ourAllValues[i - 5];
		}
	}
	return ourAllValues[15];
}

void ZLTextLineSpacingOptionEntry::onAccept(const std::string &value) {
	if (value == ourAllValuesPlusBase[0]) {
		myOption.setValue(0.0);
	} else {
		for (int i = 5; i <= 20; ++i) {
			if (value == ourAllValues[i - 5]) {
				myOption.setValue(i / 10.0);
			}
		}
	}
}

ZLTextLineSpaceOptionEntry::ZLTextLineSpaceOptionEntry(ZLIntegerOption &option, const ZLResource &resource, bool allowBase) : myResource(resource), myOption(option), myAllowBase(allowBase) {
	if (ourAllValuesPlusBase.empty()) {
		for (int i = 5; i <= 20; ++i) {
			ourAllValues.push_back(std::string() + (char)(i / 10 + '0') + '.' + (char)(i % 10 + '0'));
		}
		ourAllValuesPlusBase.push_back(myResource[KEY_UNCHANGED].value());
		ourAllValuesPlusBase.insert(ourAllValuesPlusBase.end(), ourAllValues.begin(), ourAllValues.end());
	}
}

ZLTextLineSpaceOptionEntry::~ZLTextLineSpaceOptionEntry() {
}

const std::vector<std::string> &ZLTextLineSpaceOptionEntry::values() const { return myAllowBase ? ourAllValuesPlusBase : ourAllValues; }

const std::string &ZLTextLineSpaceOptionEntry::initialValue() const {
	const int value = myOption.value();
	if (value == -1) {
		return ourAllValuesPlusBase[0];
	}
	const int index = std::max(0, std::min(15, (value + 5) / 10 - 5));
	return ourAllValues[index];
}

void ZLTextLineSpaceOptionEntry::onAccept(const std::string &value) {
	if (value == ourAllValuesPlusBase[0]) {
		myOption.setValue(-1);
	} else {
		for (int i = 5; i <= 20; ++i) {
			if (value == ourAllValues[i - 5]) {
				myOption.setValue(10 * i);
				break;
			}
		}
	}
}

std::vector<std::string> ZLTextAlignmentOptionEntry::ourValues4;
std::vector<std::string> ZLTextAlignmentOptionEntry::ourValues5;

std::vector<std::string> &ZLTextAlignmentOptionEntry::values4() const {
	if (ourValues4.empty()) {
		ourValues4.push_back(myResource[KEY_LEFT].value());
		ourValues4.push_back(myResource[KEY_RIGHT].value());
		ourValues4.push_back(myResource[KEY_CENTER].value());
		ourValues4.push_back(myResource[KEY_JUSTIFY].value());
	}
	return ourValues4;
}

std::vector<std::string> &ZLTextAlignmentOptionEntry::values5() const {
	if (ourValues5.empty()) {
		ourValues5.push_back(myResource[KEY_UNCHANGED].value());
		ourValues5.push_back(myResource[KEY_LEFT].value());
		ourValues5.push_back(myResource[KEY_RIGHT].value());
		ourValues5.push_back(myResource[KEY_CENTER].value());
		ourValues5.push_back(myResource[KEY_JUSTIFY].value());
	}
	return ourValues5;
}

ZLTextAlignmentOptionEntry::ZLTextAlignmentOptionEntry(ZLIntegerOption &option, const ZLResource &resource, bool allowUndefined) : myResource(resource), myOption(option), myAllowUndefined(allowUndefined) {
}

ZLTextAlignmentOptionEntry::~ZLTextAlignmentOptionEntry() {
}

const std::vector<std::string> &ZLTextAlignmentOptionEntry::values() const { return myAllowUndefined ? values5() : values4(); }

const std::string &ZLTextAlignmentOptionEntry::initialValue() const {
	unsigned int value = myOption.value();
	if (value >= values5().size()) {
		value = 0;
	}
	return values5()[value];
}

void ZLTextAlignmentOptionEntry::onAccept(const std::string &value) {
	for (unsigned int i = 0; i < values5().size(); ++i) {
		if (values5()[i] == value) {
			myOption.setValue(i);
			break;
		}
	}
}
