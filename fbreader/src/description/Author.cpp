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

#include <ZLUnicodeUtil.h>

#include "Author.h"

bool MultiAuthor::isSingle() const {
	return false;
}

const std::string &SingleAuthor::displayName() const {
	return myDisplayName;
}

const std::string &SingleAuthor::sortKey() const {
	return mySortKey;
}

bool SingleAuthor::isSingle() const {
	return true;
}

void MultiAuthor::addAuthor(AuthorPtr author) {
	myAuthors.push_back(author);
	myDisplayName.erase();
	mySortKey.erase();
}

const std::string &MultiAuthor::displayName() const {
	if (myDisplayName.empty() && !myAuthors.empty()) {
		myDisplayName = myAuthors.front()->displayName();
		for (unsigned int i = 1; i < myAuthors.size(); ++i) {
			myDisplayName += ", ";
			myDisplayName += myAuthors[i]->displayName();
		}
	}
	return myDisplayName;
}

const std::string &MultiAuthor::sortKey() const {
	if (mySortKey.empty() && !myAuthors.empty()) {
		mySortKey = myAuthors.front()->sortKey();
		for (unsigned int i = 1; i < myAuthors.size(); ++i) {
			const std::string &key = myAuthors[i]->sortKey();
			if (key < mySortKey) {
				mySortKey = key;
			}
		}
	}
	return mySortKey;
}
