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

#include <cctype>

#include <ZLStringUtil.h>

#include "StyleSheetParser.h"

StyleSheetParser::StyleSheetParser(StyleSheetTable &table) : myTable(table), myReadState(TAG_NAME), myInsideComment(false) {
}

void StyleSheetParser::reset() {
	myWord.erase();
	myAttributeName.erase();
	myReadState = TAG_NAME;
	myInsideComment = false;
	myElement = StyleSheetTable::Element();
}

void StyleSheetParser::parse(const char *text, int len) {
	const char *start = text;
	const char *end = text + len;
	for (const char *ptr = start; ptr != end; ++ptr) {
		if (isspace(*ptr)) {
			if (start != ptr) {
				myWord.append(start, ptr - start);
			}
			processWord(myWord);
			myWord.erase();
			start = ptr + 1;
		} else if (isControlSymbol(*ptr)) {
			if (start != ptr) {
				myWord.append(start, ptr - start);
			}
			processWord(myWord);
			myWord.erase();
			processControl(*ptr);
			start = ptr + 1;
		}
	}
}

bool StyleSheetParser::isControlSymbol(const char symbol) {
	switch (symbol) {
		case '{':
		case '}':
		case ';':
		case ':':
			return true;
		default:
			return false;
	}
}

void StyleSheetParser::processControl(const char control) {
	switch (control) {
		case '{':
			myReadState = (myReadState == TAG_NAME) ? ATTRIBUTE_NAME : BROKEN;
			break;
		case '}':
			if (myReadState != BROKEN) {
				myTable.addElement(myElement);
			}
			myReadState = TAG_NAME;
			myElement = StyleSheetTable::Element();
			break;
		case ';':
			myReadState =
				((myReadState == ATTRIBUTE_VALUE) ||
				 (myReadState == ATTRIBUTE_NAME)) ? ATTRIBUTE_NAME : BROKEN;
			break;
		case ':':
			myReadState = (myReadState == ATTRIBUTE_NAME) ? ATTRIBUTE_VALUE : BROKEN;
			break;
	}
}

void StyleSheetParser::processWord(std::string &word) {
	while (!word.empty()) {
		int index = word.find(myInsideComment ? "*/" : "/*");
		if (!myInsideComment) {
			if (index == -1) {
				processWordWithoutComments(word);
			} else if (index > 0) {
				processWordWithoutComments(word.substr(0, index));
			}
		}
		if (index == -1) {
			break;
		}
		myInsideComment = !myInsideComment;
		word.erase(0, index + 2);
	}
}
	
void StyleSheetParser::processWordWithoutComments(const std::string &word) {	
	switch (myReadState) {
		case TAG_NAME:
		{
			int index = word.find('.');
			if (index == -1) {
				myElement.first.TagName = word;
			} else {
				myElement.first.TagName = word.substr(0, index);
				myElement.first.ClassName = word.substr(index + 1);
			}
			break;
		}
		case ATTRIBUTE_NAME:
			myAttributeName = word;
			myElement.second[myAttributeName].clear();
			break;
		case ATTRIBUTE_VALUE:
			myElement.second[myAttributeName].push_back(word);
			break;
		case BROKEN:
			break;
	}
}
