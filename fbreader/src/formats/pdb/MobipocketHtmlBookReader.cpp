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

#include <algorithm>

#include <ZLFile.h>
#include <ZLFileImage.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "MobipocketHtmlBookReader.h"
#include "MobipocketStream.h"
#include "../html/HtmlTagActions.h"
#include "../../bookmodel/BookModel.h"

class MobipocketHtmlImageTagAction : public HtmlTagAction {

public:
	MobipocketHtmlImageTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrTagAction : public HtmlTagAction {

public:
	MobipocketHtmlHrTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlHrefTagAction : public HtmlHrefTagAction {

public:
	MobipocketHtmlHrefTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlGuideTagAction : public HtmlTagAction {

public:
	MobipocketHtmlGuideTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

class MobipocketHtmlReferenceTagAction : public HtmlTagAction {

public:
	MobipocketHtmlReferenceTagAction(HtmlBookReader &reader);
	void run(const HtmlReader::HtmlTag &tag);
};

MobipocketHtmlImageTagAction::MobipocketHtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlImageTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "RECINDEX") {
				int index = atoi(tag.Attributes[i].Value.c_str());
				if (index > 0) {
					int &imageCounter = ((MobipocketHtmlBookReader&)myReader).myImageCounter;
					imageCounter = std::max(imageCounter, index);
					bool stopParagraph = bookReader().paragraphIsOpen();
					if (stopParagraph) {
						bookReader().endParagraph();
					}
					std::string id;
					ZLStringUtil::appendNumber(id, index);
					bookReader().addImageReference(id);
					if (stopParagraph) {
						bookReader().beginParagraph();
					}
				}
				break;
			}
		}
	}
}

MobipocketHtmlHrTagAction::MobipocketHtmlHrTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlHrTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		if (bookReader().contentsParagraphIsOpen()) {
			bookReader().endContentsParagraph();
			bookReader().exitTitle();
		}
		bookReader().insertEndOfSectionParagraph();
	}
}

MobipocketHtmlHrefTagAction::MobipocketHtmlHrefTagAction(HtmlBookReader &reader) : HtmlHrefTagAction(reader) {
}

void MobipocketHtmlHrefTagAction::run(const HtmlReader::HtmlTag &tag) {
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "FILEPOS") {
				const std::string &value = tag.Attributes[i].Value;
				if (!value.empty()) {
					std::string label = "&";
					int ivalue = atoi(value.c_str());
					if (ivalue > 0) {
						((MobipocketHtmlBookReader&)myReader).myFileposReferences.insert(ivalue);
						ZLStringUtil::appendNumber(label, ivalue);
						setHyperlinkType(INTERNAL_HYPERLINK);
						bookReader().addHyperlinkControl(INTERNAL_HYPERLINK, label);
						return;
					}
				}
			}
		}
	}
	HtmlHrefTagAction::run(tag);
}

MobipocketHtmlGuideTagAction::MobipocketHtmlGuideTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlGuideTagAction::run(const HtmlReader::HtmlTag &tag) {
	((MobipocketHtmlBookReader&)myReader).myInsideGuide = tag.Start;
}

MobipocketHtmlReferenceTagAction::MobipocketHtmlReferenceTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlReferenceTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	if (reader.myInsideGuide) {
		std::string title;
		std::string filepos;
		for (size_t i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "TITLE") {
				title = tag.Attributes[i].Value;
			} else if (tag.Attributes[i].Name == "FILEPOS") {
				filepos = tag.Attributes[i].Value;
			}
		}
		if (!title.empty() && !filepos.empty()) {
			int pos = atoi(filepos.c_str());
			if (pos > 0) {
				reader.myTocEntries[pos] = title;
			}
		}
	}
}

shared_ptr<HtmlTagAction> MobipocketHtmlBookReader::createAction(const std::string &tag) {
	if (tag == "IMG") {
		return new MobipocketHtmlImageTagAction(*this);
	} else if (tag == "HR") {
		return new MobipocketHtmlHrTagAction(*this);
	} else if (tag == "A") {
		return new MobipocketHtmlHrefTagAction(*this);
	} else if (tag == "GUIDE") {
		return new MobipocketHtmlGuideTagAction(*this);
	} else if (tag == "REFERENCE") {
		return new MobipocketHtmlReferenceTagAction(*this);
	}
	return HtmlBookReader::createAction(tag);
}

void MobipocketHtmlBookReader::startDocumentHandler() {
	HtmlBookReader::startDocumentHandler();
	myImageCounter = 0;
	myInsideGuide = false;
	myFileposReferences.clear();
	myPositionToParagraphMap.clear();
	myTocEntries.clear();
}

bool MobipocketHtmlBookReader::tagHandler(const HtmlTag &tag) {
	myPositionToParagraphMap.push_back(std::pair<int,int>(tag.Offset, myBookReader.model().bookTextModel()->paragraphsNumber()));
	return HtmlBookReader::tagHandler(tag);
}

MobipocketHtmlBookReader::MobipocketHtmlBookReader(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlBookReader("", model, format, encoding), myFileName(fileName) {
	setBuildTableOfContent(false);
}

void MobipocketHtmlBookReader::readDocument(ZLInputStream &stream) {
	HtmlBookReader::readDocument(stream);

	shared_ptr<ZLInputStream> fileStream = ZLFile(myFileName).inputStream();
	bool found = false;
	int index = 0;
	if (!fileStream.isNull() && fileStream->open()) {
		char bu[2];
		std::pair<int,int> firstImageLocation = ((MobipocketStream&)stream).imageLocation(0);
		fileStream->seek(firstImageLocation.first, false);
		while ((firstImageLocation.first > 0) && (firstImageLocation.second > 0)) {
			if (firstImageLocation.second > 2) {
				fileStream->read(bu, 2);
				if ((strncmp(bu, "BM", 2) == 0) || (strncmp(bu, "GI", 2) == 0)) {
					found = true;
					break;
				}
				fileStream->seek(firstImageLocation.second - 2, false);
			} else {
				fileStream->seek(firstImageLocation.second, false);
			}
			index++;
			firstImageLocation = ((MobipocketStream&)stream).imageLocation(index);
		}
		fileStream->close();
	}

	if (found) {
		for (int i = 0; i < myImageCounter; i++) {
			std::pair<int,int> imageLocation = ((MobipocketStream&)stream).imageLocation(i + index);
			if ((imageLocation.first > 0) && (imageLocation.second > 0)) {
				std::string id;
				ZLStringUtil::appendNumber(id, i + 1);
				myBookReader.addImage(id, new ZLFileImage("image/auto", myFileName, imageLocation.first, imageLocation.second));
			}
		}
	}

	std::vector<std::pair<int, int> >::const_iterator jt = myPositionToParagraphMap.begin();
	for (std::set<int>::const_iterator it = myFileposReferences.begin(); it != myFileposReferences.end(); ++it) {
		while (jt != myPositionToParagraphMap.end() && jt->first < *it) {
			++jt;
		}
		if (jt == myPositionToParagraphMap.end()) {
			break;
		}
		std::string label = "&";
		ZLStringUtil::appendNumber(label, *it);
		myBookReader.addHyperlinkLabel(label, jt->second);
	}

	jt = myPositionToParagraphMap.begin();
	for (std::map<int,std::string>::const_iterator it = myTocEntries.begin(); it != myTocEntries.end(); ++it) {
		while (jt != myPositionToParagraphMap.end() && jt->first < it->first) {
			++jt;
		}
		if (jt == myPositionToParagraphMap.end()) {
			break;
		}
		myBookReader.beginContentsParagraph(jt->second);
		myBookReader.addContentsData(it->second);
		myBookReader.endContentsParagraph();
	}
}
