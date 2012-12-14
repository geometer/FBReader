/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "OptionsDialog.h"
#include "BookTagEntry.h"

#include "../library/Tag.h"

BookTagEntry::BookTagEntry(OptionsDialog &dialog, std::string initialTag, bool &visible) :
		ZLComboOptionEntry(true), myInfoDialog(dialog), myInitialValue(initialTag) {

	myEmpty = myInitialValue.empty();
	setVisible(visible || !myEmpty);
	if (visible && myEmpty) {
		visible = false;
	}
}

const std::string &BookTagEntry::initialValue() const {
	return myInitialValue;
}

const std::vector<std::string> &BookTagEntry::values() const {
	if (myValues.empty()) {
		myValues.push_back("");
		Tag::collectTagNames(myValues);
	}
	return myValues;
}

void BookTagEntry::onAccept(const std::string &value) {
	if (isVisible() && !value.empty()) {
		myInfoDialog.myNewTags.push_back(value);
	}
}

bool BookTagEntry::useOnValueEdited() const {
	return true;
}

void BookTagEntry::onValueEdited(const std::string &value) {
	onValueChanged(value);
}

void BookTagEntry::onValueSelected(int index) {
	onValueChanged(myValues[index]);
}

void BookTagEntry::onValueChanged(const std::string &value) {
	if (!myInfoDialog.myTagsDone || !isVisible()) {
		return;
	}

	myEmpty = value.empty();
	if (myEmpty) {
		for (std::size_t i = 0; i < myInfoDialog.myTagEntries.size(); ++i) {
			BookTagEntry &entry = *myInfoDialog.myTagEntries[i];
			if (entry.myEmpty && entry.isVisible() && this != &entry) {
				entry.setVisible(false);
			}
		}
	} else {
		std::size_t i, lastvisible = (std::size_t) -1;
		for (i = 0; i < myInfoDialog.myTagEntries.size(); ++i) {
			BookTagEntry &entry = *myInfoDialog.myTagEntries[i];
			if (entry.isVisible()) {
				lastvisible = i;
				if (entry.myEmpty) {
					break;
				}
			}
		}
		if (i == myInfoDialog.myTagEntries.size()) {
			if (lastvisible + 1 < i) {
				BookTagEntry &entry = *myInfoDialog.myTagEntries[lastvisible + 1];
				entry.setVisible(true);
			}
		}
	}
}
