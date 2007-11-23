/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLLanguageList.h>

#include "ZLLanguageOptionEntry.h"

ZLLanguageOptionEntry::ZLLanguageOptionEntry(ZLStringOption &languageOption) : myLanguageOption(languageOption) {
	const std::vector<std::string> &codes = ZLLanguageList::languageCodes();
	const std::string initialCode = myLanguageOption.value();
	for (std::vector<std::string>::const_iterator it = codes.begin(); it != codes.end(); ++it) {
		myValues.push_back(ZLLanguageList::languageName(*it));
		if (initialCode == *it) {
			myInitialValue = myValues.back();
		}
	}
	if (myInitialValue.empty()) {
		myInitialValue = ZLLanguageList::languageName("other");
	}
}

const std::string &ZLLanguageOptionEntry::initialValue() const {
	return myInitialValue;
}

const std::vector<std::string> &ZLLanguageOptionEntry::values() const {
	return myValues;
}

void ZLLanguageOptionEntry::onAccept(const std::string &value) {
	const std::vector<std::string> &codes = ZLLanguageList::languageCodes();
	const size_t index = std::find(myValues.begin(), myValues.end(), value) - myValues.begin();
	myLanguageOption.setValue((index < codes.size()) ? codes[index] : codes.back());
}
