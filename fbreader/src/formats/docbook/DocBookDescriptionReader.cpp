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

#include <ZLInputStream.h>
#include <ZLStringUtil.h>

#include "DocBookDescriptionReader.h"

#include "../../library/Book.h"
#include "../../library/Author.h"

DocBookDescriptionReader::DocBookDescriptionReader(Book &book) : myBook(book) {
	myReadTitle = false;
	myReadAuthor = false;
	for (int i = 0; i < 3; ++i) {
		myReadAuthorName[i] = false;
	}
	myBook.setLanguage("en");
	myDepth = 0;
}

void DocBookDescriptionReader::characterDataHandler(const char *text, size_t len) {
	if (myReadTitle) {
		myBook.setTitle(myBook.title() + std::string(text, len));
	} else {
		for (int i = 0; i < 3; ++i) {
			if (myReadAuthorName[i]) {
				myAuthorNames[i].append(text, len);
				break;
			}
		}
	}
}

void DocBookDescriptionReader::startElementHandler(int tag, const char **) {
	++myDepth;
	switch (tag) {
		case _SECT1:
			myReturnCode = true;
			myDoBreak = true;
			break;
		case _TITLE:
			if (myDepth == 2) {
				myReadTitle = true;
			}
			break;
		case _AUTHOR:
			if (myDepth == 3) {
				myReadAuthor = true;
			}
			break;
		case _FIRSTNAME:
			if (myReadAuthor) {
				myReadAuthorName[0] = true;
			}
			break;
		case _OTHERNAME:
			if (myReadAuthor) {
				myReadAuthorName[1] = true;
			}
			break;
		case _SURNAME:
			if (myReadAuthor) {
				myReadAuthorName[2] = true;
			}
			break;
		default:
			break;
	}
}

void DocBookDescriptionReader::endElementHandler(int tag) {
	--myDepth;
	switch (tag) {
		case _TITLE:
			myReadTitle = false;
			break;
		case _AUTHOR: {
				ZLStringUtil::stripWhiteSpaces(myAuthorNames[0]);
				ZLStringUtil::stripWhiteSpaces(myAuthorNames[1]);
				ZLStringUtil::stripWhiteSpaces(myAuthorNames[2]);
				std::string fullName = myAuthorNames[0];
				if (!fullName.empty() && !myAuthorNames[1].empty()) {
					fullName += ' ';
				}
				fullName += myAuthorNames[1];
				if (!fullName.empty() && !myAuthorNames[2].empty()) {
					fullName += ' ';
				}
				fullName += myAuthorNames[2];
				shared_ptr<Author> author = Author::create(fullName, myAuthorNames[2]);
				if (!author.isNull()) {
					myBook.authors().add( author );
				}
			}
			myAuthorNames[0].erase();
			myAuthorNames[1].erase();
			myAuthorNames[2].erase();
			myReadAuthor = false;
			break;
		case _FIRSTNAME:
			myReadAuthorName[0] = false;
			break;
		case _OTHERNAME:
			myReadAuthorName[1] = false;
			break;
		case _SURNAME:
			myReadAuthorName[2] = false;
			break;
		default:
			break;
	}
}

bool DocBookDescriptionReader::readMetaInfo(shared_ptr<ZLInputStream> stream) {
	bool code = readDocument(stream);
	if (myBook.authors().empty()) {
		myBook.authors().push_back( new Author() );
	}
	return code;
}
