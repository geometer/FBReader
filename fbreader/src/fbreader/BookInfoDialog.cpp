/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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
#include <ZLStringUtil.h>

#include <optionEntries/ZLStringInfoEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLLanguageOptionEntry.h>

#include "BookInfoDialog.h"

#include "../library/Library.h"
#include "../encodingOption/EncodingOptionEntry.h"
#include "../library/Book.h"
#include "../library/Tag.h"
#include "../library/Author.h"

static const size_t AUTHOR_ENTRIES_POOL_SIZE = 64;
static const size_t TAG_ENTRIES_POOL_SIZE = 64;

class AuthorDisplayNameEntry : public ZLComboOptionEntry {

public:
	AuthorDisplayNameEntry(BookInfoDialog &dialog, shared_ptr<Author> initialAuthor, bool &visible);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

	bool useOnValueEdited() const;
	void onValueEdited(const std::string &value);
	void onValueSelected(int index);

private:
	void onValueChanged(const std::string &value);

private:
	BookInfoDialog &myInfoDialog;
	mutable std::vector<std::string> myValues;
	shared_ptr<Author> myCurrentAuthor;

	std::string myInitialValue;
	bool myEmpty;

friend class SeriesTitleEntry;
friend class BookInfoApplyAction;
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
	std::set<std::string> myOriginalValuesSet;
	mutable std::vector<std::string> myValues;
};


class BookIndexEntry : public ZLSpinOptionEntry {

public:
	static const int MIN_NUMBER;
	static const int MAX_NUMBER;
	
public:
	BookIndexEntry(BookInfoDialog &dialog);

	int initialValue() const;
	int minValue() const;
	int maxValue() const;
	int step() const;
	void onAccept(int value);

private:
	BookInfoDialog &myInfoDialog;
};




AuthorDisplayNameEntry::AuthorDisplayNameEntry(BookInfoDialog &dialog, shared_ptr<Author> initialAuthor, bool &visible) : 
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
	myCurrentAuthor = (((size_t)index) < authors.size()) ? authors[index] : 0;
	myInfoDialog.mySeriesTitleEntry->resetView();
	onValueChanged(myValues[index]);
}

void AuthorDisplayNameEntry::onValueChanged(const std::string &value) {
	if (!myInfoDialog.myAuthorsDone || !isVisible()) {
		return;
	}

	myEmpty = value.empty();
	if (myEmpty) {
		for (size_t i = 0; i < myInfoDialog.myAuthorEntries.size(); ++i) {
			AuthorDisplayNameEntry &entry = *myInfoDialog.myAuthorEntries[i];
			if (entry.myEmpty && entry.isVisible() && this != &entry) {
				entry.setVisible(false);
			}
		}
	} else {
		size_t i, lastvisible = (size_t) -1;
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




SeriesTitleEntry::SeriesTitleEntry(BookInfoDialog &dialog) : ZLComboOptionEntry(true), myInfoDialog(dialog) {
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
	myInfoDialog.myBookIndexEntry->setVisible(index != 0);
}

bool SeriesTitleEntry::useOnValueEdited() const {
	return true;
}

void SeriesTitleEntry::onValueEdited(const std::string &value) {
	myInfoDialog.myBookIndexEntry->setVisible(!value.empty());
}


const int BookIndexEntry::MIN_NUMBER = 0;
const int BookIndexEntry::MAX_NUMBER = 100;

BookIndexEntry::BookIndexEntry(BookInfoDialog &dialog) : myInfoDialog(dialog) {
}

int BookIndexEntry::initialValue() const {
	return myInfoDialog.myBook->indexInSeries();
}

int BookIndexEntry::minValue() const {
	return MIN_NUMBER;
}

int BookIndexEntry::maxValue() const {
	return MAX_NUMBER;
}

int BookIndexEntry::step() const {
	return 1;
}

void BookIndexEntry::onAccept(int value) {
	Book &book = *myInfoDialog.myBook;
	book.setSeries(book.seriesTitle(), value);
}

class BookTitleEntry : public ZLStringOptionEntry {

public:
	BookTitleEntry(BookInfoDialog &dialog);

	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	BookInfoDialog &myInfoDialog;
};

BookTitleEntry::BookTitleEntry(BookInfoDialog &dialog) : myInfoDialog(dialog) {
}

const std::string &BookTitleEntry::initialValue() const {
	return myInfoDialog.myBook->title();
}

void BookTitleEntry::onAccept(const std::string &value) {
	myInfoDialog.myBook->setTitle(value);
}






class BookEncodingEntry : public AbstractEncodingEntry {

public:
	BookEncodingEntry(BookInfoDialog &dialog);

	void onAcceptValue(const std::string &value);

private:
	BookInfoDialog &myInfoDialog;
};

BookEncodingEntry::BookEncodingEntry(BookInfoDialog &dialog) : 
	AbstractEncodingEntry(dialog.myBook->encoding()), 
	myInfoDialog(dialog) {
}

void BookEncodingEntry::onAcceptValue(const std::string &value) {
	myInfoDialog.myBook->setEncoding(value);
}



class BookLanguageEntry : public ZLAbstractLanguageOptionEntry {

public:
	BookLanguageEntry(BookInfoDialog &dialog, const std::vector<std::string> &languageCodes);

	void onAcceptCode(const std::string &code);

private:
	BookInfoDialog &myInfoDialog;
};

BookLanguageEntry::BookLanguageEntry(BookInfoDialog &dialog, const std::vector<std::string> &languageCodes) : 
	ZLAbstractLanguageOptionEntry(dialog.myBook->language(), languageCodes),
	myInfoDialog(dialog) {
}

void BookLanguageEntry::onAcceptCode(const std::string &code) {
	myInfoDialog.myBook->setLanguage(code);
}





class BookTagEntry : public ZLComboOptionEntry {

public:
	BookTagEntry(BookInfoDialog &dialog, std::string initialTag, bool &visible);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

	bool useOnValueEdited() const;
	void onValueEdited(const std::string &value);
	void onValueSelected(int index);

private:
	void onValueChanged(const std::string &value);

private:
	BookInfoDialog &myInfoDialog;
	std::string myInitialValue;
	bool myEmpty;

	mutable std::vector<std::string> myValues;
};

BookTagEntry::BookTagEntry(BookInfoDialog &dialog, std::string initialTag, bool &visible) : 
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
		for (size_t i = 0; i < myInfoDialog.myTagEntries.size(); ++i) {
			BookTagEntry &entry = *myInfoDialog.myTagEntries[i];
			if (entry.myEmpty && entry.isVisible() && this != &entry) {
				entry.setVisible(false);
			}
		}
	} else {
		size_t i, lastvisible = (size_t) -1;
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

class BookInfoApplyAction : public ZLRunnable {

public:
	BookInfoApplyAction(BookInfoDialog &dialog);
	void run();

private:
	BookInfoDialog &myInfoDialog;
};

BookInfoApplyAction::BookInfoApplyAction(BookInfoDialog &dialog) : myInfoDialog(dialog) {}

void BookInfoApplyAction::run() {
	Book &book = *myInfoDialog.myBook;

	AuthorList authors;
	for (size_t i = 0; i < myInfoDialog.myAuthorEntries.size(); ++i) {
		shared_ptr<Author> a = myInfoDialog.myAuthorEntries[i]->myCurrentAuthor;
		if (!a.isNull() &&
				std::find(authors.begin(), authors.end(), a) == authors.end()) {
			authors.push_back(a);
		}
	}

	book.removeAllAuthors();
	for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		book.addAuthor(*it);
	}

	book.removeAllTags();
	for (size_t i = 0; i < myInfoDialog.myNewTags.size(); ++i) {
		book.addTag(myInfoDialog.myNewTags[i]);
	}

	Library::Instance().updateBook(myInfoDialog.myBook);
}

BookInfoDialog::BookInfoDialog(shared_ptr<Book> book) : myBook(book) {
	myDialog = ZLDialogManager::Instance().createOptionsDialog(ZLResourceKey("InfoDialog"), new BookInfoApplyAction(*this));

	ZLDialogContent &commonTab = myDialog->createTab(ZLResourceKey("Common"));
	commonTab.addOption(ZLResourceKey("file"), 
		new ZLStringInfoEntry(ZLFile::fileNameToUtf8(ZLFile(book->filePath()).path()))
	);
	commonTab.addOption(ZLResourceKey("title"), new BookTitleEntry(*this));

	myEncodingEntry = new BookEncodingEntry(*this);
	myEncodingSetEntry =
		(myEncodingEntry->initialValue() != "auto") ?
		new EncodingSetEntry(*(EncodingEntry*)myEncodingEntry) : 0;
	std::vector<std::string> languageCodes = ZLLanguageList::languageCodes();
	languageCodes.push_back("de-traditional");
	myLanguageEntry = new BookLanguageEntry(*this, languageCodes);
	mySeriesTitleEntry = new SeriesTitleEntry(*this);
	myBookIndexEntry = new BookIndexEntry(*this);

	commonTab.addOption(ZLResourceKey("language"), myLanguageEntry);
	if (myEncodingSetEntry != 0) {
		commonTab.addOption(ZLResourceKey("encodingSet"), myEncodingSetEntry);
	}
	commonTab.addOption(ZLResourceKey("encoding"), myEncodingEntry);

	initAuthorEntries();

	ZLDialogContent &seriesTab = myDialog->createTab(ZLResourceKey("Series"));
	seriesTab.addOption(ZLResourceKey("seriesTitle"), mySeriesTitleEntry);
	seriesTab.addOption(ZLResourceKey("bookIndex"), myBookIndexEntry);

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

	initTagEntries();

	shared_ptr<FormatPlugin> plugin = PluginCollection::Instance().plugin(*book);
	if (!plugin.isNull()) {
		myFormatInfoPage = plugin->createInfoPage(*myDialog, book->filePath());
	}
}

void BookInfoDialog::initTagEntries() {
	bool visible = true;
	const TagList &tags = myBook->tags();
	myTagsDone = false;
	myTagsTab = &myDialog->createTab(ZLResourceKey("Tags"));
	for (size_t i = 0; i < TAG_ENTRIES_POOL_SIZE; ++i) {
		std::string tag = (i < tags.size()) ? tags[i]->fullName() : "";
		BookTagEntry *entry = new BookTagEntry(*this, tag, visible);
		myTagEntries.push_back(entry);
		myTagsTab->addOption(ZLResourceKey("tags"), entry);
	}
	myTagsDone = true;
}

void BookInfoDialog::initAuthorEntries() {
	bool visible = true;
	const AuthorList &authors = myBook->authors();
	myAuthorsDone = false;
	myAuthorsTab = &myDialog->createTab(ZLResourceKey("Authors"));
	for (size_t i = 0; i < AUTHOR_ENTRIES_POOL_SIZE; ++i) {
		shared_ptr<Author> author = (i < authors.size()) ? authors[i] : 0;
		AuthorDisplayNameEntry *entry = new AuthorDisplayNameEntry(*this, author, visible);
		myAuthorEntries.push_back(entry);
		myAuthorsTab->addOption(ZLResourceKey("authorDisplayName"), entry);
	}
	myAuthorsDone = true;
}

