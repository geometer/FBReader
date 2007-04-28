/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

class HtmlSectionTagAction : public HtmlTagAction {

protected:
	HtmlSectionTagAction(HtmlSectionReader &reader);
	HtmlSectionReader &reader();
};

class HtmlSectionHrefTagAction : public HtmlSectionTagAction {

public:
	HtmlSectionHrefTagAction(HtmlSectionReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlSectionImageTagAction : public HtmlSectionTagAction {

public:
	HtmlSectionImageTagAction(HtmlSectionReader &reader);
	void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

HtmlSectionReader::HtmlSectionReader(BookModel &model, const PlainTextFormat &format, const std::string &encoding, shared_ptr<CHMFileInfo> info, CHMReferenceCollection &collection) : HtmlBookReader("", model, format, encoding), myInfo(info), myReferenceCollection(collection) {
	addAction("IMG", new HtmlSectionImageTagAction(*this));
	addAction("A", new HtmlSectionHrefTagAction(*this));
	//addAction("PRE", 0);
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

HtmlSectionTagAction::HtmlSectionTagAction(HtmlSectionReader &reader) : HtmlTagAction(reader) {
}

HtmlSectionReader &HtmlSectionTagAction::reader() {
	return (HtmlSectionReader&)myReader;
}

HtmlSectionHrefTagAction::HtmlSectionHrefTagAction(HtmlSectionReader &reader) : HtmlSectionTagAction(reader) {
}

void HtmlSectionHrefTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
	if (start) {
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			if (attributes[i].Name == "NAME") {
				bookReader().addHyperlinkLabel(ZLUnicodeUtil::toLower(reader().myCurrentSectionName + '#' + attributes[i].Value));
			} else if ((reader().hyperlinkType() == REGULAR) && (attributes[i].Name == "HREF")) {
				const std::string &value = attributes[i].Value;
				if (!value.empty()) {
					if (MiscUtil::isReference(value)) {
						bookReader().addHyperlinkControl(EXTERNAL_HYPERLINK, value);
						reader().setHyperlinkType(EXTERNAL_HYPERLINK);
					} else {
						const int index = value.find('#');
						std::string sectionName = (index == -1) ? value : value.substr(0, index);
						sectionName = ZLUnicodeUtil::toLower(MiscUtil::decodeHtmlURL(sectionName));
						if (sectionName.empty()) {
							sectionName = reader().myCurrentSectionName;
						} else {
							sectionName = reader().myReferenceCollection.addReference(sectionName, true);
						}
						bookReader().addHyperlinkControl(
							INTERNAL_HYPERLINK, ZLUnicodeUtil::toLower((index == -1) ? sectionName : (sectionName + value.substr(index)))
						);
						reader().setHyperlinkType(INTERNAL_HYPERLINK);
					}
				}
			}
		}
	} else if (reader().hyperlinkType() != REGULAR) {
		bookReader().addControl(reader().hyperlinkType(), false);
		reader().setHyperlinkType(REGULAR);
	}
}

HtmlSectionImageTagAction::HtmlSectionImageTagAction(HtmlSectionReader &reader) : HtmlSectionTagAction(reader) {
}

void HtmlSectionImageTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
	if (start) {
		//bookReader().endParagraph();
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			if (attributes[i].Name == "SRC") {
				std::string fileName = MiscUtil::decodeHtmlURL(attributes[i].Value);
				fileName = CHMReferenceCollection::fullReference(reader().myReferenceCollection.prefix(), fileName);
				fileName = ZLUnicodeUtil::toLower(fileName);
				bookReader().addImageReference(fileName);
				bookReader().addImage(fileName, new CHMFileImage(reader().myInfo, fileName));
				break;
			}
		}
		//bookReader().beginParagraph();
	}
}
