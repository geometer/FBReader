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

HtmlDescriptionReader::HtmlDescriptionReader(BookDescription &description) : DescriptionReader(description) {
}

void HtmlDescriptionReader::startDocumentHandler() {
	myReadTitle = false;
}

void HtmlDescriptionReader::endDocumentHandler() {
	addAuthor("Html", "Book", "Writer");
	if (myTitle.empty()) {
		myTitle.append("Html book");
	}
	addToTitle(myTitle.c_str(), myTitle.length());
	addToLanguage("en", 2);
	validateDescription();
}

bool HtmlDescriptionReader::tagHandler(HtmlTag tag) {
	if (tag.Code == _TITLE) {
		myReadTitle = tag.Start && myTitle.empty();
	}
	return tag.Code != _BODY;
}

bool HtmlDescriptionReader::characterDataHandler(const char *text, int len) {
	if (myReadTitle) {
		myTitle.append(text, len);
	}
	return true;
}
