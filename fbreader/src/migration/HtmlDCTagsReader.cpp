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

#include <ZLStringUtil.h>

#include "HtmlDCTagsReader.h"

HtmlDCTagsReader::HtmlDCTagsReader(BookInfo &info) : HtmlReader(info.EncodingOption.value()), myInfo(info) {
}

bool HtmlDCTagsReader::tagHandler(const HtmlReader::HtmlTag &tag) {
	if (tag.Name == "BODY") {
		return false;
	} else if (tag.Name == "DC:SUBJECT") {
		myReadTag = tag.Start;
		ZLStringUtil::stripWhiteSpaces(myBuffer);
		if (!tag.Start && !myBuffer.empty()) {
			if (!myTagList.empty()) {
				myTagList += ",";
			}
			myTagList += myBuffer;
			myBuffer.erase();
		}
	}
	return true;
}

void HtmlDCTagsReader::startDocumentHandler() {
	myReadTag = false;
}

void HtmlDCTagsReader::endDocumentHandler() {
	myInfo.TagsOption.setValue(myTagList);
}

bool HtmlDCTagsReader::characterDataHandler(const char *text, int len, bool convert) {
	if (myReadTag) {
		if (convert) {
			myConverter->convert(myBuffer, text, text + len);
		} else {
			myBuffer.append(text, len);
		}
	}
	return true;
}
