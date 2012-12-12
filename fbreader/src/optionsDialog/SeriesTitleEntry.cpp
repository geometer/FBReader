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

#include <algorithm>

#include "OptionsDialog.h"
#include "AuthorDisplayNameEntry.h"
#include "SeriesIndexEntry.h"
#include "SeriesTitleEntry.h"

#include "../library/Library.h"
#include "../library/Book.h"
#include "../library/Author.h"

SeriesTitleEntry::SeriesTitleEntry(OptionsDialog &dialog) : ZLComboOptionEntry(true), myInfoDialog(dialog) {
	const AuthorList &authors = myInfoDialog.myBook->authors();
	myOriginalValuesSet.insert(initialValue());
	myOriginalValuesSet.insert("");
	const Library &library = Library::Instance();
	for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		library.collectSeriesTitles(*it, myOriginalValuesSet);
	}
}

const std::string &SeriesTitleEntry::initialValue() const {
	return myInfoDialog.myBook->seriesTitle();
}

const std::vector<std::string> &SeriesTitleEntry::values() const {
	std::set<std::string> valuesSet(myOriginalValuesSet);

	const Library &library = Library::Instance();
	const AuthorList &authors = myInfoDialog.myBook->authors();
	for (std::vector<AuthorDisplayNameEntry*>::const_iterator it = myInfoDialog.myAuthorEntries.begin(); it != myInfoDialog.myAuthorEntries.end(); ++it) {
		shared_ptr<Author> currentAuthor = (*it)->myCurrentAuthor;
		if (!currentAuthor.isNull() && std::find(authors.begin(), authors.end(), currentAuthor) == authors.end()) {
			library.collectSeriesTitles(currentAuthor, valuesSet);
		}
	}

	/*myValues.clear();
	for (std::set<std::string>::const_iterator it = valuesSet.begin(); it != valuesSet.end(); ++it) {
		myValues.push_back(*it);
	}*/
	myValues.assign(valuesSet.begin(), valuesSet.end());
	return myValues;
}

void SeriesTitleEntry::onAccept(const std::string &value) {
	Book &book = *myInfoDialog.myBook;
	book.setSeries(value, book.indexInSeries());
}

void SeriesTitleEntry::onValueSelected(int index) {
	myInfoDialog.mySeriesIndexEntry->setVisible(index != 0);
}

bool SeriesTitleEntry::useOnValueEdited() const {
	return true;
}

void SeriesTitleEntry::onValueEdited(const std::string &value) {
	myInfoDialog.mySeriesIndexEntry->setVisible(!value.empty());
}
