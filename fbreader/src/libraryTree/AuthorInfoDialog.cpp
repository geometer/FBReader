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

#include <algorithm>

#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLOptionEntry.h>

#include "AuthorInfoDialog.h"

#include "../library/Library.h"
#include "../library/Author.h"
#include "../library/Comparators.h"

class AuthorNameEntry : public ZLComboOptionEntry {

public:
	AuthorNameEntry(AuthorInfoDialog &dialog, size_t index);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);
	void onValueSelected(int index);

public:
	size_t Index;

private:
	AuthorInfoDialog &myInfoDialog;
	mutable std::vector<std::string> myValues;
	std::string myValue;
};

class AuthorSortKeyEntry : public ZLStringOptionEntry {
public:
	AuthorSortKeyEntry(AuthorInfoDialog &dialog);

	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	AuthorInfoDialog &myInfoDialog;
};

AuthorNameEntry::AuthorNameEntry(AuthorInfoDialog &dialog, size_t index) : ZLComboOptionEntry(true), Index(index), myInfoDialog(dialog) {
}

const std::string &AuthorNameEntry::initialValue() const {
	return myInfoDialog.initialAuthor()->name();
}

void AuthorNameEntry::onAccept(const std::string &value) {
	myInfoDialog.name() = value;
}

const std::vector<std::string> &AuthorNameEntry::values() const {
	if (myValues.empty()) {
		const AuthorList &authors = myInfoDialog.authors();
		for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
			myValues.push_back((*it)->name());
		}
	}
	return myValues;
}

void AuthorNameEntry::onValueSelected(int index) {
	Index = index;
	myInfoDialog.sortKeyEntry().resetView();
}

AuthorSortKeyEntry::AuthorSortKeyEntry(AuthorInfoDialog &dialog) : myInfoDialog(dialog) {
}

const std::string &AuthorSortKeyEntry::initialValue() const {
	const AuthorList &authors = myInfoDialog.authors();
	size_t index = std::min(myInfoDialog.nameEntry().Index, authors.size() - 1);
	return authors[index]->sortKey();
}

void AuthorSortKeyEntry::onAccept(const std::string &value) {
	myInfoDialog.sortKey() = value;
}



bool AuthorInfoDialog::run(shared_ptr<Author> author) {
	AuthorInfoDialog dlg(author);
	if (dlg.dialog().run()) {
		dlg.dialog().acceptValues();
		shared_ptr<Author> newAuthor = Author::getAuthor(dlg.name(), dlg.sortKey());
		Library::Instance().replaceAuthor(author, newAuthor);
		return true;
	}
	return false;
}


AuthorInfoDialog::AuthorInfoDialog(shared_ptr<Author> author) : myInitialAuthor(author) {
	const AuthorList &authors = Library::Instance().authors();
	for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if (!it->isNull()) {
			myAuthors.push_back(*it);
		}
	}
	AuthorList::iterator jt = std::lower_bound(myAuthors.begin(), myAuthors.end(), myInitialAuthor, AuthorComparator());
	if (jt == myAuthors.end() || *jt != myInitialAuthor) {
		myAuthors.insert(jt, myInitialAuthor);
	}

	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("AuthorInfoDialog"));

	myNameEntry = new AuthorNameEntry(*this, jt - myAuthors.begin());
	mySortKeyEntry = new AuthorSortKeyEntry(*this);

	myDialog->addOption(ZLResourceKey("name"), myNameEntry);
	myDialog->addOption(ZLResourceKey("sortKey"), mySortKeyEntry);

	myDialog->addButton(ZLDialogManager::OK_BUTTON, true);
	myDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
}

ZLDialog &AuthorInfoDialog::dialog() {
	return *myDialog;
}

shared_ptr<Author> AuthorInfoDialog::initialAuthor() {
	return myInitialAuthor;
}

const AuthorList &AuthorInfoDialog::authors() const {
	return myAuthors;
}

std::string &AuthorInfoDialog::name() {
	return myName;
}

std::string &AuthorInfoDialog::sortKey() {
	return mySortKey;
}

AuthorNameEntry &AuthorInfoDialog::nameEntry() {
	return *myNameEntry;
}

AuthorSortKeyEntry &AuthorInfoDialog::sortKeyEntry() {
	return *mySortKeyEntry;
}
