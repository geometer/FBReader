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
#include "../formats/fb2/FB2TagManager.h"

FB2MigrationReader::FB2MigrationReader(BookInfo &info, bool updateSeries) : myInfo(info), myUpdateSeries(updateSeries), myUpdateTags(info.TagsOption.value().empty()) {
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
					std::string seriesName = name;
					ZLStringUtil::stripWhiteSpaces(seriesName);
					myInfo.SeriesNameOption.setValue(seriesName);
					const char *number = attributeValue(attributes, "number");
					myInfo.NumberInSeriesOption.setValue((number != 0) ? atoi(number) : 0);
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
				ZLStringUtil::stripWhiteSpaces(myGenreBuffer);
				if (!myGenreBuffer.empty()) {
					const std::vector<std::string> &tags =
						FB2TagManager::instance().humanReadableTags(myGenreBuffer);
					if (!tags.empty()) {
						myTags.insert(tags.begin(), tags.end());
					} else {
						myTags.insert(myGenreBuffer);
					}
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
	readDocument(fileName);
	if (myUpdateTags) {
		std::string tagList;
		for (std::set<std::string>::const_iterator it = myTags.begin(); it != myTags.end(); ++it) {
			if (it != myTags.begin()) {
				tagList += ",";
			}
			tagList += *it;
		}
		myInfo.TagsOption.setValue(tagList);
	}
}
