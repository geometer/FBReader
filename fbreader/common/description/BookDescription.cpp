/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLInputStream.h>

#include "BookDescription.h"
#include "DescriptionReader.h"
#include "Author.h"

#include "../formats/ReaderCollection.h"

BookDescription *BookDescription::create(const std::string &fileName) {
	BookDescription *description = new BookDescription(fileName);
	if (!description->myIsValid) {
		delete description;
		return 0;
	}
	return description;
}

BookDescription::BookDescription(const BookDescription &description) {
	myFileName = description.myFileName;
	myAuthor = description.myAuthor->createCopy();
	myTitle = description.myTitle;
	myLanguage = description.myLanguage;
	myIsValid = description.myIsValid;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myIsValid = false;
	myAuthor = 0;
	
	ZLInputStream *stream = ZLInputStream::createStream(myFileName);
	if (stream != 0) {
		DescriptionReader *reader = ReaderCollection::createDescriptionReader(*this);
		if (reader != 0) {
			reader->readDescription(*stream);
			delete reader;
		}
		delete stream;
	}

	if (!myIsValid) {
		return;
	}
	if (myTitle.empty()) {
		int slashPos = myFileName.find_last_of('/');
		int dotPos = myFileName.find_first_of('.', slashPos + 1);
		myTitle = myFileName.substr(slashPos + 1, dotPos - slashPos - 1);
	}
	if (myAuthor == 0) {
		myAuthor = new DummyAuthor();
	}
}

BookDescription::~BookDescription() {
	if (myAuthor != 0) {
		delete myAuthor;
	}
}
