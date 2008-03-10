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

#include <stdlib.h>

#include <ZLInputStream.h>
#include <ZLStringUtil.h>

#include "FB2MigrationReader.h"

FB2MigrationReader::FB2MigrationReader(BookDescription &description, bool updateSeries) : myDescription(description), myUpdateSeries(updateSeries), myUpdateTags(description.tags().empty()) {
}

void FB2MigrationReader::characterDataHandler(const char *text, int len) {
	if (myReadState == READ_GENRE) {
		myGenreBuffer.append(text, len);
	}
}

void FB2MigrationReader::startElementHandler(int tag, const char **attributes) {
	switch (tag) {
		case _BODY:
			interrupt();
			break;
		case _TITLE_INFO:
			myReadState = READ_SOMETHING;
			break;
		case _GENRE:
			if ((myReadState == READ_SOMETHING) && myUpdateTags) {
				myReadState = READ_GENRE;
			}
			break;
		case _SEQUENCE:
			if ((myReadState == READ_SOMETHING) && myUpdateSeries) {
				const char *name = attributeValue(attributes, "name");
				if (name != 0) {
					std::string sequenceName = name;
					ZLStringUtil::stripWhiteSpaces(sequenceName);
					myDescription.sequenceName() = sequenceName;
					const char *number = attributeValue(attributes, "number");
					myDescription.numberInSequence() = (number != 0) ? atoi(number) : 0;
				}
			}
			break;
		default:
			break;
	}
}

void FB2MigrationReader::endElementHandler(int tag) {
	switch (tag) {
		case _TITLE_INFO:
			myReadState = READ_NOTHING;
			break;
		case _GENRE:
			if (myReadState == READ_GENRE) {
				if (!myGenreBuffer.empty()) {
					myDescription.addTag(myGenreBuffer);
					myGenreBuffer.erase();
				}
				myReadState = READ_SOMETHING;
			}
			break;
		default:
			break;
	}
}

void FB2MigrationReader::doRead(const std::string &fileName) {
	myReadState = READ_NOTHING;
	myGenreBuffer.erase();
	readDocument(fileName);
}
