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

#include <abstract/ZLEncodingConverter.h>

#include "InfoOptions.h"

static const char *languageShortNames[] = {
	"en",
	"fr",
	"de",
	"de-traditional",
	"ru",
	0
};

std::vector<std::string> LanguageEntry::ourLanguages;

static std::vector<std::string> AUTO_ENCODING;

StringInfoEntry::StringInfoEntry(const std::string &name, const std::string &value) : myName(name), myValue(value) {
	setActive(false);
}

StringInfoEntry::~StringInfoEntry() {
}

const std::string &StringInfoEntry::name() const {
	return myName;
}

const std::string &StringInfoEntry::initialValue() const {
	return myValue;
}

void StringInfoEntry::onAccept(const std::string&) {
}

EncodingEntry::EncodingEntry(const std::string &name, ZLStringOption &encodingOption) : myName(name), myEncodingOption(encodingOption) {
	if (initialValue() == "auto") {
		setActive(false);
	}
}

EncodingEntry::~EncodingEntry() {
}

const std::vector<std::string> &EncodingEntry::values() const {
	if (initialValue() == "auto") {
		if (AUTO_ENCODING.empty()) {
			AUTO_ENCODING.push_back("auto");
		}
		return AUTO_ENCODING;
	}
	return ZLEncodingConverter::knownEncodings();
}

const std::string &EncodingEntry::name() const {
	return myName;
}

const std::string &EncodingEntry::initialValue() const {
	return myEncodingOption.value();
}

void EncodingEntry::onAccept(const std::string &value) {
	myEncodingOption.setValue(value);
}

LanguageEntry::LanguageEntry(const std::string &name, ZLStringOption &encodingOption) : myName(name), myLanguageOption(encodingOption) {
}

LanguageEntry::~LanguageEntry() {
}

const std::string &LanguageEntry::initialValue() const {
	const std::string &lang = myLanguageOption.value();
	int i = 0;
	for (; languageShortNames[i] != 0; i++) {
		if (lang == languageShortNames[i]) {
			break;
		}
	}
	return values()[i];
}

const std::vector<std::string> &LanguageEntry::values() const {
	if (ourLanguages.empty()) {
		ourLanguages.push_back("English");
		ourLanguages.push_back("French");
		ourLanguages.push_back("German");
		ourLanguages.push_back("German (traditional)");
		ourLanguages.push_back("Russian");
		ourLanguages.push_back("none");
	}
	return ourLanguages;
}

const std::string &LanguageEntry::name() const {
	return myName;
}

void LanguageEntry::onAccept(const std::string &value) {
	for (int i = 0; languageShortNames[i] != 0; i++) {
		if (value == values()[i]) {
			myLanguageOption.setValue(languageShortNames[i]);
			return;
		}
	}
	myLanguageOption.setValue("none");
}
