/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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
#include "AuthorDisplayNameEntry.h"
#include "SeriesTitleEntry.h"

#include "../library/Library.h"
#include "../library/Author.h"

AuthorDisplayNameEntry::AuthorDisplayNameEntry(OptionsDialog &dialog, shared_ptr<Author> initialAuthor, bool &visible) :
	ZLComboOptionEntry(true), myInfoDialog(dialog), myCurrentAuthor(initialAuthor) {

	if (myCurrentAuthor.isNull()) {
		myInitialValue = "";
		myEmpty = true;
	} else {
		myInitialValue = myCurrentAuthor->name();
		myEmpty = myInitialValue.empty();
	}
	setVisible(visible || !myEmpty);
	if (visible && myEmpty) {
		visible = false;
	}
}

const std::string &AuthorDisplayNameEntry::initialValue() const {
	return myInitialValue;
}

const std::vector<std::string> &AuthorDisplayNameEntry::values() const {
	if (myValues.empty()) {
		const std::string &initial = initialValue();
		bool addInitial = true;
		const AuthorList &authors = Library::Instance().authors();
		for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
			if (it->isNull()) {
				continue;
			}
			const std::string name = (*it)->name();
			if (addInitial && (name == initial)) {
				addInitial = false;
			}
			myValues.push_back(name);
		}
		if (addInitial) {
			myValues.push_back(initial);
		}
	}
	return myValues;
}

void AuthorDisplayNameEntry::onAccept(const std::string &value) {
	if (!isVisible() || value.empty()) {
		myCurrentAuthor = 0;
		return;
	}
	if (!myCurrentAuthor.isNull() && value == myCurrentAuthor->name()) {
		//myCurrentAuthor = myCurrentAuthor;
		return;
	}
	myCurrentAuthor = Author::getAuthor(value);
}

bool AuthorDisplayNameEntry::useOnValueEdited() const {
	return true;
}

void AuthorDisplayNameEntry::onValueEdited(const std::string &value) {
	onValueChanged(value);
}

void AuthorDisplayNameEntry::onValueSelected(int index) {
	const AuthorList &authors = Library::Instance().authors();
	myCurrentAuthor = (((std::size_t)index) < authors.size()) ? authors[index] : 0;
	myInfoDialog.mySeriesTitleEntry->resetView();
	onValueChanged(myValues[index]);
}

void AuthorDisplayNameEntry::onValueChanged(const std::string &value) {
	if (!myInfoDialog.myAuthorsDone || !isVisible()) {
		return;
	}

	myEmpty = value.empty();
	if (myEmpty) {
		for (std::size_t i = 0; i < myInfoDialog.myAuthorEntries.size(); ++i) {
			AuthorDisplayNameEntry &entry = *myInfoDialog.myAuthorEntries[i];
			if (entry.myEmpty && entry.isVisible() && this != &entry) {
				entry.setVisible(false);
			}
		}
	} else {
		std::size_t i, lastvisible = (std::size_t) -1;
		for (i = 0; i < myInfoDialog.myAuthorEntries.size(); ++i) {
			AuthorDisplayNameEntry &entry = *myInfoDialog.myAuthorEntries[i];
			if (entry.isVisible()) {
				lastvisible = i;
				if (entry.myEmpty) {
					break;
				}
			}
		}
		if (i == myInfoDialog.myAuthorEntries.size()) {
			if (lastvisible + 1 < i) {
				AuthorDisplayNameEntry &entry = *myInfoDialog.myAuthorEntries[lastvisible + 1];
				entry.setVisible(true);
			}
			// else pool is over
		}
	}
}
