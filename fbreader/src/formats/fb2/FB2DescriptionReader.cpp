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

#include "FB2DescriptionReader.h"
#include "FB2TagManager.h"

FB2DescriptionReader::FB2DescriptionReader(BookDescription &description) : myDescription(description) {
	myDescription.clearAuthor();
	myDescription.title().erase();
	myDescription.language().erase();
	myDescription.removeAllTags();
}

void FB2DescriptionReader::characterDataHandler(const char *text, int len) {
	switch (myReadState) {
		case READ_TITLE:
			myDescription.title().append(text, len);
			break;
		case READ_LANGUAGE:
			myDescription.language().append(text, len);
			break;
		case READ_AUTHOR_NAME_0:
			myAuthorNames[0].append(text, len);
			break;
		case READ_AUTHOR_NAME_1:
			myAuthorNames[1].append(text, len);
			break;
		case READ_AUTHOR_NAME_2:
			myAuthorNames[2].append(text, len);
			break;
		case READ_GENRE:
			myGenreBuffer.append(text, len);
			break;
		default:
			break;
	}
}

void FB2DescriptionReader::startElementHandler(int tag, const char **attributes) {
	switch (tag) {
		case _BODY:
			myReturnCode = true;
			interrupt();
			break;
		case _TITLE_INFO:
			myReadState = READ_SOMETHING;
			break;
		case _BOOK_TITLE:
			if (myReadState == READ_SOMETHING) {
				myReadState = READ_TITLE;
			}
			break;
		case _GENRE:
			if (myReadState == READ_SOMETHING) {
				myReadState = READ_GENRE;
			}
			break;
		case _AUTHOR:
			if (myReadState == READ_SOMETHING) {
				myReadState = READ_AUTHOR;
			}
			break;
		case _LANG:
			if (myReadState == READ_SOMETHING) {
				myReadState = READ_LANGUAGE;
			}
			break;
		case _FIRST_NAME:
			if (myReadState == READ_AUTHOR) {
				myReadState = READ_AUTHOR_NAME_0;
			}
			break;
		case _MIDDLE_NAME:
			if (myReadState == READ_AUTHOR) {
				myReadState = READ_AUTHOR_NAME_1;
			}
			break;
		case _LAST_NAME:
			if (myReadState == READ_AUTHOR) {
				myReadState = READ_AUTHOR_NAME_2;
			}
			break;
		case _SEQUENCE:
			if (myReadState == READ_SOMETHING) {
				const char *name = attributeValue(attributes, "name");
				if (name != 0) {
					std::string seriesName = name;
					ZLStringUtil::stripWhiteSpaces(seriesName);
					myDescription.seriesName() = seriesName;
					const char *number = attributeValue(attributes, "number");
					myDescription.numberInSeries() = (number != 0) ? atoi(number) : 0;
				}
			}
			break;
		default:
			break;
	}
}

void FB2DescriptionReader::endElementHandler(int tag) {
	switch (tag) {
		case _TITLE_INFO:
			myReadState = READ_NOTHING;
			break;
		case _BOOK_TITLE:
			if (myReadState == READ_TITLE) {
				myReadState = READ_SOMETHING;
			}
			break;
		case _GENRE:
			if (myReadState == READ_GENRE) {
				ZLStringUtil::stripWhiteSpaces(myGenreBuffer);
				if (!myGenreBuffer.empty()) {
					const std::vector<std::string> &tags =
						FB2TagManager::instance().humanReadableTags(myGenreBuffer);
					if (!tags.empty()) {
						for (std::vector<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it) {
							myDescription.addTag(*it, false);
						}
					} else {
						myDescription.addTag(myGenreBuffer);
					}
					myGenreBuffer.erase();
				}
				myReadState = READ_SOMETHING;
			}
			break;
		case _AUTHOR:
			if (myReadState == READ_AUTHOR) {
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
				myDescription.addAuthor(fullName, myAuthorNames[2]);
				myAuthorNames[0].erase();
				myAuthorNames[1].erase();
				myAuthorNames[2].erase();
				myReadState = READ_SOMETHING;
			}
			break;
		case _LANG:
			if (myReadState == READ_LANGUAGE) {
				myReadState = READ_SOMETHING;
			}
			break;
		case _FIRST_NAME:
			if (myReadState == READ_AUTHOR_NAME_0) {
				myReadState = READ_AUTHOR;
			}
			break;
		case _MIDDLE_NAME:
			if (myReadState == READ_AUTHOR_NAME_1) {
				myReadState = READ_AUTHOR;
			}
			break;
		case _LAST_NAME:
			if (myReadState == READ_AUTHOR_NAME_2) {
				myReadState = READ_AUTHOR;
			}
			break;
		default:
			break;
	}
}

bool FB2DescriptionReader::readDescription(const std::string &fileName) {
	myReadState = READ_NOTHING;
	for (int i = 0; i < 3; ++i) {
		myAuthorNames[i].erase();
	}
	myGenreBuffer.erase();
	return readDocument(fileName);
}
