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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include <ZLFile.h>
#include <ZLLanguageList.h>

#include <optionEntries/ZLStringInfoEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLLanguageOptionEntry.h>

#include "BookInfoDialog.h"

#include "../description/Author.h"
#include "../encodingOption/EncodingOptionEntry.h"

class AuthorSortKeyEntry : public ZLStringOptionEntry {

public:
	AuthorSortKeyEntry(BookInfoDialog &dialog);

	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	BookInfoDialog &myInfoDialog;
};

class AuthorDisplayNameEntry : public ZLComboOptionEntry {

public:
	AuthorDisplayNameEntry(BookInfoDialog &dialog);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

private:
	BookInfoDialog &myInfoDialog;
	mutable std::vector<std::string> myValues;
	AuthorPtr myCurrentAuthor;

friend class AuthorSortKeyEntry;
friend class SeriesTitleEntry;
};

class SeriesTitleEntry : public ZLComboOptionEntry {

public:
	SeriesTitleEntry(BookInfoDialog &dialog);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

	bool useOnValueEdited() const;
	void onValueEdited(const std::string &value);
	void onValueSelected(int index);

private:
	BookInfoDialog &myInfoDialog;
	AuthorPtr myOriginalAuthor;
	mutable std::vector<std::string> myValues;
};

AuthorDisplayNameEntry::AuthorDisplayNameEntry(BookInfoDialog &dialog) : ZLComboOptionEntry(true), myInfoDialog(dialog) {
}

const std::string &AuthorDisplayNameEntry::initialValue() const {
	return myInfoDialog.myBookInfo.AuthorDisplayNameOption.value();
}

const std::vector<std::string> &AuthorDisplayNameEntry::values() const {
	if (myValues.empty()) {
		const std::string &initial = initialValue();
		bool addInitial = true;
		const std::vector<AuthorPtr> &authors = myInfoDialog.myCollection.authors();
		for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
			const std::string name = (*it)->displayName();
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
	myInfoDialog.myBookInfo.AuthorDisplayNameOption.setValue(value);
}

void AuthorDisplayNameEntry::onValueSelected(int index) {
	const std::vector<AuthorPtr> &authors = myInfoDialog.myCollection.authors();
	myCurrentAuthor = authors[index];
	myInfoDialog.myAuthorSortKeyEntry->resetView();
	myInfoDialog.mySeriesTitleEntry->resetView();
}

AuthorSortKeyEntry::AuthorSortKeyEntry(BookInfoDialog &dialog) : myInfoDialog(dialog) {
}

const std::string &AuthorSortKeyEntry::initialValue() const {
	AuthorPtr currentAuthor = myInfoDialog.myAuthorDisplayNameEntry->myCurrentAuthor;
	return currentAuthor.isNull() ?
		myInfoDialog.myBookInfo.AuthorSortKeyOption.value() :
		currentAuthor->sortKey();
}

void AuthorSortKeyEntry::onAccept(const std::string &value) {
	myInfoDialog.myBookInfo.AuthorSortKeyOption.setValue(value);
}

SeriesTitleEntry::SeriesTitleEntry(BookInfoDialog &dialog) : ZLComboOptionEntry(true), myInfoDialog(dialog) {
	const std::vector<AuthorPtr> &authors = myInfoDialog.myCollection.authors();
	const std::string &authorName = myInfoDialog.myBookInfo.AuthorDisplayNameOption.value();
	const std::string &authorKey = myInfoDialog.myBookInfo.AuthorSortKeyOption.value();
	for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if ((authorName == (*it)->displayName()) &&
				(authorKey == (*it)->sortKey())) {
			myOriginalAuthor = *it;
			break;
		}
	}
}

const std::string &SeriesTitleEntry::initialValue() const {
	return myInfoDialog.myBookInfo.SeriesNameOption.value();
}

const std::vector<std::string> &SeriesTitleEntry::values() const {
	myValues.clear();
	std::set<std::string> valuesSet;
	valuesSet.insert(initialValue());
	valuesSet.insert("");
	if (!myOriginalAuthor.isNull()) {
		myInfoDialog.myCollection.collectSeriesNames(myOriginalAuthor, valuesSet);
	}
	AuthorPtr currentAuthor = myInfoDialog.myAuthorDisplayNameEntry->myCurrentAuthor;
	if (!currentAuthor.isNull() && (currentAuthor != myOriginalAuthor)) {
		myInfoDialog.myCollection.collectSeriesNames(currentAuthor, valuesSet);
	}
	for (std::set<std::string>::const_iterator it = valuesSet.begin(); it != valuesSet.end(); ++it) {
		myValues.push_back(*it);
	}
	return myValues;
}

void SeriesTitleEntry::onAccept(const std::string &value) {
	myInfoDialog.myBookInfo.SeriesNameOption.setValue(value);
}

void SeriesTitleEntry::onValueSelected(int index) {
	myInfoDialog.myBookNumberEntry->setVisible(index != 0);
}

bool SeriesTitleEntry::useOnValueEdited() const {
	return true;
}

void SeriesTitleEntry::onValueEdited(const std::string &value) {
	myInfoDialog.myBookNumberEntry->setVisible(!value.empty());
}

BookInfoDialog::BookInfoDialog(const BookCollection &collection, const std::string &fileName) : myCollection(collection), myBookInfo(fileName) {
	myDialog = ZLDialogManager::instance().createOptionsDialog(ZLResourceKey("InfoDialog"));

	ZLDialogContent &commonTab = myDialog->createTab(ZLResourceKey("Common"));
	commonTab.addOption(ZLResourceKey("file"), 
		new ZLStringInfoEntry(ZLFile::fileNameToUtf8(ZLFile(fileName).path()))
	);
	commonTab.addOption(ZLResourceKey("title"), myBookInfo.TitleOption);

	myAuthorDisplayNameEntry = new AuthorDisplayNameEntry(*this);
	myAuthorSortKeyEntry = new AuthorSortKeyEntry(*this);
	myEncodingEntry = new EncodingEntry(myBookInfo.EncodingOption);
	myEncodingSetEntry =
		(myEncodingEntry->initialValue() != "auto") ?
		new EncodingSetEntry(*(EncodingEntry*)myEncodingEntry) : 0;
	std::vector<std::string> languageCodes = ZLLanguageList::languageCodes();
	languageCodes.push_back("de-traditional");
	myLanguageEntry = new ZLLanguageOptionEntry(myBookInfo.LanguageOption, languageCodes);
	mySeriesTitleEntry = new SeriesTitleEntry(*this);
	myBookNumberEntry = new ZLSimpleSpinOptionEntry(myBookInfo.NumberInSeriesOption, 1);

	commonTab.addOption(ZLResourceKey("authorDisplayName"), myAuthorDisplayNameEntry);
	commonTab.addOption(ZLResourceKey("authorSortKey"), myAuthorSortKeyEntry);
	commonTab.addOption(ZLResourceKey("language"), myLanguageEntry);
	if (myEncodingSetEntry != 0) {
		commonTab.addOption(ZLResourceKey("encodingSet"), myEncodingSetEntry);
	}
	commonTab.addOption(ZLResourceKey("encoding"), myEncodingEntry);

	ZLDialogContent &seriesTab = myDialog->createTab(ZLResourceKey("Series"));
	seriesTab.addOption(ZLResourceKey("seriesTitle"), mySeriesTitleEntry);
	seriesTab.addOption(ZLResourceKey("bookNumber"), myBookNumberEntry);

	mySeriesTitleEntry->onValueEdited(mySeriesTitleEntry->initialValue());
	/*
	ZLOrderOptionEntry *orderEntry = new ZLOrderOptionEntry();
	orderEntry->values().push_back("First");
	orderEntry->values().push_back("Second");
	orderEntry->values().push_back("Third");
	orderEntry->values().push_back("Fourth");
	orderEntry->values().push_back("Fifth");
	seriesTab.addOption(orderEntry);
	*/

	ZLDialogContent &tagsTab = myDialog->createTab(ZLResourceKey("Tags"));
	tagsTab.addOption(ZLResourceKey("tags"), myBookInfo.TagsOption);

	FormatPlugin *plugin = PluginCollection::instance().plugin(ZLFile(fileName), false);
	if (plugin != 0) {
		myFormatInfoPage = plugin->createInfoPage(*myDialog, fileName);
	}
}
