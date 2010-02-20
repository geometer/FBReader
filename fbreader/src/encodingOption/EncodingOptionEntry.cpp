/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <shared_ptr.h>
#include <ZLEncodingConverter.h>

#include "EncodingOptionEntry.h"

#include "../library/Book.h"

AbstractEncodingEntry::AbstractEncodingEntry(const std::string &currentValue) {
	const std::string &value = currentValue;
	if (value == Book::AutoEncoding) {
		myInitialSetName = value;
		myInitialValues[value] = value;
		setActive(false);
		return;
	}

	const std::vector<shared_ptr<ZLEncodingSet> > &sets = ZLEncodingCollection::Instance().sets();
	for (std::vector<shared_ptr<ZLEncodingSet> >::const_iterator it = sets.begin(); it != sets.end(); ++it) {
		const std::vector<ZLEncodingConverterInfoPtr> &infos = (*it)->infos();
		mySetNames.push_back((*it)->name());
		std::vector<std::string> &names = myValues[(*it)->name()];
		for (std::vector<ZLEncodingConverterInfoPtr>::const_iterator jt = infos.begin(); jt != infos.end(); ++jt) {
			if ((*jt)->name() == value) {
				myInitialSetName = (*it)->name();
				myInitialValues[myInitialSetName] = (*jt)->visibleName();
			}
			names.push_back((*jt)->visibleName());
			myValueByName[(*jt)->visibleName()] = (*jt)->name();
		}
	}

	if (myInitialSetName.empty()) {
		myInitialSetName = mySetNames[0];
	}
}

const std::vector<std::string> &AbstractEncodingEntry::values() const {
	if (initialValue() == Book::AutoEncoding) {
		static std::vector<std::string> AUTO_ENCODING;
		if (AUTO_ENCODING.empty()) {
			AUTO_ENCODING.push_back(Book::AutoEncoding);
		}
		return AUTO_ENCODING;
	}
	std::map<std::string,std::vector<std::string> >::const_iterator it = myValues.find(myInitialSetName);
	return it->second;
}

const std::string &AbstractEncodingEntry::initialValue() const {
	if (myInitialValues[myInitialSetName].empty()) {
		std::map<std::string,std::vector<std::string> >::const_iterator it = myValues.find(myInitialSetName);
		myInitialValues[myInitialSetName] = it->second[0];
	}
	return myInitialValues[myInitialSetName];
}

void AbstractEncodingEntry::onAccept(const std::string &value) {
	if (initialValue() != Book::AutoEncoding) {
		onAcceptValue(myValueByName[value]);
	}
}

void AbstractEncodingEntry::onValueSelected(int index) {
	myInitialValues[myInitialSetName] = values()[index];
}





EncodingEntry::EncodingEntry(ZLStringOption &encodingOption) : 
	AbstractEncodingEntry(encodingOption.value()), 
	myEncodingOption(encodingOption) {
}

void EncodingEntry::onAcceptValue(const std::string &value) {
	myEncodingOption.setValue(value);
}




EncodingSetEntry::EncodingSetEntry(AbstractEncodingEntry &encodingEntry) : myEncodingEntry(encodingEntry) {
}

const std::string &EncodingSetEntry::initialValue() const {
	return myEncodingEntry.myInitialSetName;
}

const std::vector<std::string> &EncodingSetEntry::values() const {
	return myEncodingEntry.mySetNames;
}

void EncodingSetEntry::onValueSelected(int index) {
	myEncodingEntry.myInitialSetName = values()[index];
	myEncodingEntry.resetView();
}
