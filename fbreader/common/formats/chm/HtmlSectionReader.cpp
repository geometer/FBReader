/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "HtmlSectionReader.h"
#include "CHMReferenceCollection.h"
#include "CHMFileImage.h"

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
	myReferenceCollection.setPrefix(sectionName);
	myCurrentSectionName = sectionName;
}

void HtmlSectionReader::startDocumentHandler() {
	HtmlBookReader::startDocumentHandler();
	myBookReader.addHyperlinkLabel(myCurrentSectionName);
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
				bookReader().addHyperlinkLabel(reader().myCurrentSectionName + '#' + attributes[i].Value);
			} else if (!reader().myIsHyperlink && (attributes[i].Name == "HREF")) {
				const std::string &value = attributes[i].Value;
				if (!value.empty() &&
						(value.substr(0, 7) != "http://") &&
						(value.substr(0, 7) != "mailto:")) {
					const int index = value.find('#');
					std::string sectionName = (index == -1) ? value : value.substr(0, index);
					sectionName = HtmlReader::decodeURL(sectionName);
					if (sectionName.empty()) {
						sectionName = reader().myCurrentSectionName;
					} else {
						sectionName = reader().myReferenceCollection.addReference(sectionName, true);
					}
					bookReader().addHyperlinkControl(
						HYPERLINK, (index == -1) ? sectionName : (sectionName + value.substr(index))
					);
					reader().myIsHyperlink = true;
				}
			}
		}
	} else if (reader().myIsHyperlink) {
		bookReader().addControl(HYPERLINK, false);
		reader().myIsHyperlink = false;
	}
}

HtmlSectionImageTagAction::HtmlSectionImageTagAction(HtmlSectionReader &reader) : HtmlSectionTagAction(reader) {
}

void HtmlSectionImageTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
	if (start) {
		//bookReader().endParagraph();
		for (unsigned int i = 0; i < attributes.size(); ++i) {
			if (attributes[i].Name == "SRC") {
				std::string fileName = HtmlReader::decodeURL(attributes[i].Value);
				fileName = CHMReferenceCollection::fullReference(reader().myReferenceCollection.prefix(), fileName);
				bookReader().addImageReference(fileName);
				bookReader().addImage(fileName, new CHMFileImage(reader().myInfo, fileName));
				break;
			}
		}
		//bookReader().beginParagraph();
	}
}
