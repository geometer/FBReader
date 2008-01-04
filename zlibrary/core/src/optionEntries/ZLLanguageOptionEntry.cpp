/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <map>

#include <ZLLanguageList.h>

#include "ZLLanguageOptionEntry.h"

ZLLanguageOptionEntry::ZLLanguageOptionEntry(ZLStringOption &languageOption, const std::vector<std::string> &languageCodes) : myLanguageOption(languageOption) {
	const std::string initialCode = myLanguageOption.value();
	for (std::vector<std::string>::const_iterator it = languageCodes.begin(); it != languageCodes.end(); ++it) {
		const std::string name = ZLLanguageList::languageName(*it);
		myValuesToCodes[name] = *it;
		if (initialCode == *it) {
			myInitialValue = name;
		}
	}
	for (std::map<std::string,std::string>::const_iterator it = myValuesToCodes.begin(); it != myValuesToCodes.end(); ++it) {
		myValues.push_back(it->first);
	}
	static const std::string otherCode = "other";
	std::string otherName = ZLLanguageList::languageName(otherCode);
	myValues.push_back(otherName);
	myValuesToCodes[otherName] = otherCode;
	if (myInitialValue.empty()) {
		myInitialValue = otherName;
	}
}

const std::string &ZLLanguageOptionEntry::initialValue() const {
	return myInitialValue;
}

const std::vector<std::string> &ZLLanguageOptionEntry::values() const {
	return myValues;
}

void ZLLanguageOptionEntry::onAccept(const std::string &value) {
	myLanguageOption.setValue(myValuesToCodes[value]);
}
