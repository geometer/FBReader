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

#include "DescriptionReader.h"
#include "BookDescription.h"
#include "Author.h"

void DescriptionReader::addAuthor(const std::string &firstName, const std::string &middleName, const std::string &lastName) {
	Author *author = new SingleAuthorWith3Names(firstName, middleName, lastName);
	if (myDescription.myAuthor == 0) {
		myDescription.myAuthor = author;
	} else if (myDescription.myAuthor->isSingle()) {
		MultiAuthor *multiAuthor = new MultiAuthor();
		multiAuthor->addAuthor(myDescription.myAuthor);
		multiAuthor->addAuthor(author);
		myDescription.myAuthor = multiAuthor;
	} else {
		((MultiAuthor*)myDescription.myAuthor)->addAuthor(author);
	}
}

DescriptionReader::DescriptionReader(BookDescription &description) : myDescription(description) {
}

void DescriptionReader::addToTitle(const char *text, int len) {
	myDescription.myTitle.append(text, len);
}

void DescriptionReader::addToLanguage(const char *text, int len) {
	myDescription.myLanguage.append(text, len);
}

void DescriptionReader::validateDescription() {
	myDescription.myIsValid = true;
}
