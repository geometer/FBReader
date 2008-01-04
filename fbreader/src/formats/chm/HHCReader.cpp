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

#include <ZLUnicodeUtil.h>

#include "HHCReader.h"
#include "CHMReferenceCollection.h"

HHCReader::HHCReader(CHMReferenceCollection &collection, BookModel &model, const std::string &encoding) : HtmlReader(encoding), myReferenceCollection(collection), myBookReader(model) {
}

HHCReader::~HHCReader() {
}

void HHCReader::startDocumentHandler() {
	myBookReader.setMainTextModel();
}

void HHCReader::endDocumentHandler() {
	std::string tmp0;
	myText.swap(tmp0);
	std::string tmp1;
	myReference.swap(tmp1);
}

static const std::string UL = "UL";
static const std::string LI = "LI";
static const std::string OBJECT = "OBJECT";
static const std::string PARAM = "PARAM";
static const std::string NAME = "NAME";
static const std::string VALUE = "VALUE";
static const std::string NAME_VALUE = "Name";
static const std::string LOCAL_VALUE = "Local";

static bool isFirstChild = false;

bool HHCReader::tagHandler(const HtmlTag &tag) {
	if (tag.Start) {
		if (tag.Name == UL) {
			isFirstChild = true;
		} else if (tag.Name == LI) {
		} else if (tag.Name == OBJECT) {
			myText.erase();
			myReference.erase();
		} else if (tag.Name == PARAM) {
			std::string name;
			std::string value;
			for (std::vector<HtmlAttribute>::const_iterator it = tag.Attributes.begin(); it != tag.Attributes.end(); ++it) {
				if (it->Name == NAME) {
					name = it->Value;
				} else if (it->Name == VALUE) {
					value = it->Value;
				}
			}
			if (name == NAME_VALUE) {
				myText = value;
			} else if (name == LOCAL_VALUE) {
				myReference = myReferenceCollection.addReference(value, true);
			}
		}
	} else {
		if (tag.Name == UL) {
			myBookReader.endContentsParagraph();
		} else if (tag.Name == OBJECT) {
			if (!myText.empty() || !myReference.empty()) {
				if (!isFirstChild) {
					myBookReader.endContentsParagraph();
				} else {
					isFirstChild = false;
				}
				myBookReader.beginContentsParagraph();
				if (myText.empty()) {
					myText = "...";
				}
				myBookReader.addContentsData(myText.empty() ? "..." : myText);
				myReferenceVector.push_back(ZLUnicodeUtil::toLower(myReference));
			}
		}
	}
	return true;
}

bool HHCReader::characterDataHandler(const char*, int, bool) {
	return true;
}

void HHCReader::setReferences() {
	for (size_t i = 0; i < myReferenceVector.size(); ++i) {
		myBookReader.setReference(i, myBookReader.model().label(myReferenceVector[i]).ParagraphNumber);
	}
}
