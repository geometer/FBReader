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

#include "HtmlDescriptionReader.h"

HtmlDescriptionReader::HtmlDescriptionReader(BookDescription &description) : myDescription(description) {
}

void HtmlDescriptionReader::startDocumentHandler() {
	myReadTitle = false;
}

void HtmlDescriptionReader::endDocumentHandler() {
	myDescription.addAuthor("Html", "Book", "Writer");
	if (myDescription.title().empty()) {
		myDescription.title() = "Html book";
	} else {
		myConverter->convertString(myDescription.title());
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

bool HtmlDescriptionReader::tagHandler(HtmlTag tag) {
	if (tag.Code == _TITLE) {
		myReadTitle = tag.Start && myDescription.title().empty();
	}
	return tag.Code != _BODY;
}

bool HtmlDescriptionReader::characterDataHandler(const char *text, int len) {
	if (myReadTitle) {
		myDescription.title().append(text, len);
	}
	return true;
}
