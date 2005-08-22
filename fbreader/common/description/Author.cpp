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

#include <abstract/ZLUnicodeUtil.h>

#include "Author.h"

Author *SingleAuthorWith3Names::createCopy() const {
	return new SingleAuthorWith3Names(myFirstName, myMiddleName, myLastName);
}

Author *MultiAuthor::createCopy() const {
	MultiAuthor *copy = new MultiAuthor();
	for (std::vector<Author*>::const_iterator it = myAuthors.begin(); it != myAuthors.end(); it++) {
		copy->addAuthor((*it)->createCopy());
	}
	return copy;
}

const std::string &SingleAuthorWith3Names::displayName() const {
	if (myDisplayName.empty()) {
		if (!myFirstName.empty()) {
			myDisplayName = myFirstName;
		}
		if (!myMiddleName.empty()) {
			if (!myDisplayName.empty()) {
				myDisplayName += ' ';
			}
			myDisplayName += myMiddleName;
		}
		if (!myLastName.empty()) {
			if (!myDisplayName.empty()) {
				myDisplayName += ' ';
			}
			myDisplayName += myLastName;
		}
	}
	if (myDisplayName.empty()) {
		myDisplayName = "Unknown Author";
	}
	return myDisplayName;
}

const std::string &SingleAuthorWith3Names::sortKey() const {
	if (mySortKey.empty()) {
		mySortKey = ZLUnicodeUtil::toLower(myLastName);
	}
	if (mySortKey.empty()) {
		mySortKey = "Unknown Author";
	}
	return mySortKey;
}

MultiAuthor::~MultiAuthor() {
	for (std::vector<Author*>::const_iterator it = myAuthors.begin(); it != myAuthors.end(); it++) {
		delete *it;
	}
}

void MultiAuthor::addAuthor(Author *author) {
	myAuthors.push_back(author);
	myDisplayName.erase();
	mySortKey.erase();
}

const std::string &MultiAuthor::displayName() const {
	if (myDisplayName.empty() && !myAuthors.empty()) {
		myDisplayName = myAuthors.front()->displayName();
		for (unsigned int i = 1; i < myAuthors.size(); i++) {
			myDisplayName += ", ";
			myDisplayName += myAuthors[i]->displayName();
		}
	}
	return myDisplayName;
}

const std::string &MultiAuthor::sortKey() const {
	if (mySortKey.empty() && !myAuthors.empty()) {
		mySortKey = myAuthors.front()->sortKey();
		for (unsigned int i = 1; i < myAuthors.size(); i++) {
			const std::string &key = myAuthors[i]->sortKey();
			if (key < mySortKey) {
				mySortKey = key;
			}
		}
	}
	return mySortKey;
}
