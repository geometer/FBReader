/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLPaintContext.h>

#include "TextStyleOptions.h"
#include "TextStyle.h"

static std::string FAMILY_STRING = "Family";
static std::string LINE_SPACING_STRING = "Line Spacing";
static std::string BASE_STRING = "<base>";

std::vector<std::string> FontFamilyOptionEntry::ourAllFamilies;
std::vector<std::string> FontFamilyOptionEntry::ourAllFamiliesPlusBase;

std::vector<std::string> LineSpacingOptionEntry::ourAllValues;
std::vector<std::string> LineSpacingOptionEntry::ourAllValuesPlusBase;

FontFamilyOptionEntry::FontFamilyOptionEntry(const ZLStringOption &option, const ZLPaintContext &context, bool allowBase) : myOption(option), myAllowBase(allowBase) {
	if (ourAllFamiliesPlusBase.empty()) {
		ourAllFamilies = context.fontFamilies();
		ourAllFamiliesPlusBase.push_back(BASE_STRING);
		ourAllFamiliesPlusBase.insert(ourAllFamiliesPlusBase.end(), ourAllFamilies.begin(), ourAllFamilies.end());
	}
	std::string value = option.value();
	if (!value.empty()) {
		option.setValue(context.realFontFamilyName(value));
	}
}

const std::string &FontFamilyOptionEntry::name() const {
	return FAMILY_STRING;
}

const std::string &FontFamilyOptionEntry::initialValue() const {
	const std::string &value = myOption.value();
	return value.empty() ? BASE_STRING : value;
}

void FontFamilyOptionEntry::onAccept(const std::string &value) const {
	myOption.setValue((value == BASE_STRING) ? std::string() : value);
}

LineSpacingOptionEntry::LineSpacingOptionEntry(const ZLDoubleOption &option, bool allowBase) : myOption(option), myAllowBase(allowBase) {
	if (ourAllValuesPlusBase.empty()) {
		for (int i = 5; i <= 20; i++) {
			ourAllValues.push_back(std::string() + (char)(i / 10 + '0') + '.' + (char)(i % 10 + '0'));
		}
		ourAllValuesPlusBase.push_back(BASE_STRING);
		ourAllValuesPlusBase.insert(ourAllValuesPlusBase.end(), ourAllValues.begin(), ourAllValues.end());
	}
}

const std::string &LineSpacingOptionEntry::name() const {
	return LINE_SPACING_STRING;
}

const std::string &LineSpacingOptionEntry::initialValue() const {
	int value = (int)(10 * myOption.value() + 0.5);
	if (value == 0) {
		return BASE_STRING;
	}
	for (int i = 5; i < 20; i++) {
		if (value <= i) {
			return ourAllValues[i - 5];
		}
	}
	return ourAllValues[15];
}

void LineSpacingOptionEntry::onAccept(const std::string &value) const {
	if (value == BASE_STRING) {
		myOption.setValue(0.0);
	} else {
		for (int i = 5; i <= 20; i++) {
			if (value == ourAllValues[i - 5]) {
				myOption.setValue(i / 10.0);
			}
		}
	}
}

std::vector<std::string> AlignmentOptionEntry::ourValues4;
std::vector<std::string> AlignmentOptionEntry::ourValues5;

std::vector<std::string> &AlignmentOptionEntry::values4() {
	if (ourValues4.empty()) {
		ourValues4.push_back("Left");
		ourValues4.push_back("Right");
		ourValues4.push_back("Center");
		ourValues4.push_back("Justify");
	}
	return ourValues4;
}

std::vector<std::string> &AlignmentOptionEntry::values5() {
	if (ourValues5.empty()) {
		ourValues5.push_back(BASE_STRING);
		ourValues5.push_back("Left");
		ourValues5.push_back("Right");
		ourValues5.push_back("Center");
		ourValues5.push_back("Justify");
	}
	return ourValues5;
}

static std::string ALIGNMENT_STRING = "Alignment";

const std::string &AlignmentOptionEntry::name() const {
	return ALIGNMENT_STRING;
}

const std::string &AlignmentOptionEntry::initialValue() const {
	unsigned int value = myOption.value();
	if (value >= values5().size()) {
		value = 0;
	}
	return values5()[value];
}

void AlignmentOptionEntry::onAccept(const std::string &value) const {
	for (unsigned int i = 0; i < values5().size(); i++) {
		if (values5()[i] == value) {
			myOption.setValue(i);
			break;
		}
	}
}
