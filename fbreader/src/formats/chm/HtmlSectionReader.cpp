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

#include "HtmlSectionReader.h"
#include "CHMReferenceCollection.h"
#include "CHMFileImage.h"
#include "../util/MiscUtil.h"
#include "../html/HtmlTagActions.h"

class HtmlSectionHrefTagAction : public HtmlHrefTagAction {

public:
	HtmlSectionHrefTagAction(HtmlSectionReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class HtmlSectionImageTagAction : public HtmlTagAction {

public:
	HtmlSectionImageTagAction(HtmlSectionReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

shared_ptr<HtmlTagAction> HtmlSectionReader::createAction(const std::string &tag) {
	if (tag == "IMG") {
		return new HtmlSectionImageTagAction(*this);
	} else if (tag == "A") {
		return new HtmlSectionHrefTagAction(*this);
	}
	return HtmlBookReader::createAction(tag);
}

HtmlSectionReader::HtmlSectionReader(BookModel &model, const PlainTextFormat &format, const std::string &encoding, shared_ptr<CHMFileInfo> info, CHMReferenceCollection &collection) : HtmlBookReader("", model, format, encoding), myInfo(info), myReferenceCollection(collection) {
	setBuildTableOfContent(false);
}

void HtmlSectionReader::setSectionName(const std::string &sectionName) {
	myCurrentSectionName = ZLUnicodeUtil::toLower(sectionName);
	myReferenceCollection.setPrefix(myCurrentSectionName);
}

void HtmlSectionReader::startDocumentHandler() {
	HtmlBookReader::startDocumentHandler();
	myBookReader.addHyperlinkLabel(ZLUnicodeUtil::toLower(myCurrentSectionName));
}

void HtmlSectionReader::endDocumentHandler() {
	HtmlBookReader::endDocumentHandler();
	myBookReader.insertEndOfTextParagraph();
}

HtmlSectionHrefTagAction::HtmlSectionHrefTagAction(HtmlSectionReader &reader) : HtmlHrefTagAction(reader) {
}

void HtmlSectionHrefTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		HtmlSectionReader &reader = (HtmlSectionReader&)myReader;
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "NAME") {
				bookReader().addHyperlinkLabel(ZLUnicodeUtil::toLower(reader.myCurrentSectionName + '#' + tag.Attributes[i].Value));
			} else if ((hyperlinkType() == REGULAR) && (tag.Attributes[i].Name == "HREF")) {
				const std::string &value = tag.Attributes[i].Value;
				if (!value.empty()) {
					if (MiscUtil::isReference(value)) {
						bookReader().addHyperlinkControl(EXTERNAL_HYPERLINK, value);
						setHyperlinkType(EXTERNAL_HYPERLINK);
					} else {
						const int index = value.find('#');
						std::string sectionName = (index == -1) ? value : value.substr(0, index);
						sectionName = ZLUnicodeUtil::toLower(MiscUtil::decodeHtmlURL(sectionName));
						if (sectionName.empty()) {
							sectionName = reader.myCurrentSectionName;
						} else {
							sectionName = reader.myReferenceCollection.addReference(sectionName, true);
						}
						bookReader().addHyperlinkControl(
							INTERNAL_HYPERLINK, ZLUnicodeUtil::toLower((index == -1) ? sectionName : (sectionName + value.substr(index)))
						);
						setHyperlinkType(INTERNAL_HYPERLINK);
					}
				}
			}
		}
	} else if (hyperlinkType() != REGULAR) {
		bookReader().addControl(hyperlinkType(), false);
		setHyperlinkType(REGULAR);
	}
}

HtmlSectionImageTagAction::HtmlSectionImageTagAction(HtmlSectionReader &reader) : HtmlTagAction(reader) {
}

void HtmlSectionImageTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		//bookReader().endParagraph();
		HtmlSectionReader &reader = (HtmlSectionReader&)myReader;
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "SRC") {
				std::string fileName = MiscUtil::decodeHtmlURL(tag.Attributes[i].Value);
				fileName = CHMReferenceCollection::fullReference(reader.myReferenceCollection.prefix(), fileName);
				fileName = ZLUnicodeUtil::toLower(fileName);
				bookReader().addImageReference(fileName);
				bookReader().addImage(fileName, new CHMFileImage(reader.myInfo, fileName));
				break;
			}
		}
		//bookReader().beginParagraph();
	}
}
