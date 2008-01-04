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

#include <ZLInputStream.h>

#include "DocBookDescriptionReader.h"

DocBookDescriptionReader::DocBookDescriptionReader(BookDescription &description) : myDescription(description) {
	myReadTitle = false;
	myReadAuthor = false;
	for (int i = 0; i < 3; ++i) {
		myReadAuthorName[i] = false;
	}
	myDescription.language() = "en";
	myDepth = 0;
}

void DocBookDescriptionReader::characterDataHandler(const char *text, int len) {
	if (myReadTitle) {
		myDescription.title().append(text, len);
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
		case _AUTHOR:
			myDescription.addAuthor(myAuthorNames[0], myAuthorNames[1], myAuthorNames[2]);
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

bool DocBookDescriptionReader::readDescription(shared_ptr<ZLInputStream> stream) {
	return readDocument(stream);
}
