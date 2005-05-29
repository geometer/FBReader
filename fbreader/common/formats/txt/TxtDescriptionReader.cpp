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

#include "TxtDescriptionReader.h"

TxtDescriptionReader::TxtDescriptionReader(BookDescription &description) : myDescription(description) {
}

void TxtDescriptionReader::startDocumentHandler() {
	myReadTitle = false;
}

void TxtDescriptionReader::endDocumentHandler() {
	myDescription.addAuthor("Txt", "Book", "Writer");
	if (myDescription.title().empty()) {
		myDescription.title() = "Txt book";
	}
	if (myDescription.language() == "") {
		if ((myDescription.encoding() == "KOI8-R") ||
				(myDescription.encoding() == "windows-1251") ||
				(myDescription.encoding() == "ISO-8859-5") ||
				(myDescription.encoding() == "IBM866")) {
			myDescription.language() = "ru";
		}
	}
}

bool TxtDescriptionReader::characterDataHandler(const std::string &str) {
	if (myReadTitle) {
		myDescription.title().append(str);
	}
	return true;
}
