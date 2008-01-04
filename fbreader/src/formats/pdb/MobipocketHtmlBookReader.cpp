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
#include <string.h>

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

MobipocketHtmlBookReader::TOCReader::TOCReader(MobipocketHtmlBookReader &reader) : myReader(reader) {
	reset();
}

void MobipocketHtmlBookReader::TOCReader::reset() {
	myEntries.clear();

	myIsActive = false;
	myStartOffset = (size_t)-1;
	myEndOffset = (size_t)-1;
	myCurrentEntryText.erase();
}

bool MobipocketHtmlBookReader::TOCReader::rangeContainsPosition(size_t position) {
	return (myStartOffset <= position) && (myEndOffset > position);
}

void MobipocketHtmlBookReader::TOCReader::startReadEntry(size_t position) {
	myCurrentReference = position;
	myIsActive = true;
}

void MobipocketHtmlBookReader::TOCReader::endReadEntry() {
	if (myIsActive && !myCurrentEntryText.empty()) {
		std::string converted;
		myReader.myConverter->convert(converted, myCurrentEntryText);
		myReader.myConverter->reset();
		myEntries[myCurrentReference] = converted;
		myCurrentEntryText.erase();
	}
	myIsActive = false;
}

void MobipocketHtmlBookReader::TOCReader::appendText(const char *text, size_t len) {
	if (myIsActive) {
		myCurrentEntryText.append(text, len);
	}
}

void MobipocketHtmlBookReader::TOCReader::addReference(size_t position, const std::string &text) {
	myEntries[position] = text;
	if (rangeContainsPosition(position)) {
		setEndOffset(position);
	}
}

void MobipocketHtmlBookReader::TOCReader::setStartOffset(size_t position) {
	myStartOffset = position;
	std::map<size_t,std::string>::const_iterator it = myEntries.lower_bound(position);
	if (it != myEntries.end()) {
		++it;
		if (it != myEntries.end()) {
			myEndOffset = it->first;
		}
	}
}

void MobipocketHtmlBookReader::TOCReader::setEndOffset(size_t position) {
	myEndOffset = position;
}

const std::map<size_t,std::string> &MobipocketHtmlBookReader::TOCReader::entries() const {
	return myEntries;
}

void MobipocketHtmlHrefTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	if (tag.Start) {
		for (unsigned int i = 0; i < tag.Attributes.size(); ++i) {
			if (tag.Attributes[i].Name == "FILEPOS") {
				const std::string &value = tag.Attributes[i].Value;
				if (!value.empty()) {
					std::string label = "&";
					int intValue = atoi(value.c_str());
					if (intValue > 0) {
						if (reader.myTocReader.rangeContainsPosition(tag.Offset)) {
							reader.myTocReader.startReadEntry(intValue);
							if (reader.myTocReader.rangeContainsPosition(intValue)) {
								reader.myTocReader.setEndOffset(intValue);
							}
						}
						reader.myFileposReferences.insert(intValue);
						ZLStringUtil::appendNumber(label, intValue);
						setHyperlinkType(INTERNAL_HYPERLINK);
						bookReader().addHyperlinkControl(INTERNAL_HYPERLINK, label);
						return;
					}
				}
			}
		}
	} else {
		reader.myTocReader.endReadEntry();
	}
	HtmlHrefTagAction::run(tag);
}

MobipocketHtmlGuideTagAction::MobipocketHtmlGuideTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlGuideTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	reader.myInsideGuide = tag.Start;
}

MobipocketHtmlReferenceTagAction::MobipocketHtmlReferenceTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void MobipocketHtmlReferenceTagAction::run(const HtmlReader::HtmlTag &tag) {
	MobipocketHtmlBookReader &reader = (MobipocketHtmlBookReader&)myReader;
	if (reader.myInsideGuide) {
		std::string title;
		std::string filepos;
		bool isTocReference = false;
		for (size_t i = 0; i < tag.Attributes.size(); ++i) {
			const std::string &name = tag.Attributes[i].Name;
			const std::string &value = tag.Attributes[i].Value;
			if (name == "TITLE") {
				title = value;
			} else if (name == "FILEPOS") {
				filepos = value;
			} else if ((name == "TYPE") && (ZLUnicodeUtil::toUpper(value) == "TOC")) {
				isTocReference = true;
			}
		}
		if (!title.empty() && !filepos.empty()) {
			int position = atoi(filepos.c_str());
			if (position > 0) {
				reader.myTocReader.addReference(position, title);
				if (isTocReference) {
					reader.myTocReader.setStartOffset(position);
				}
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
	myTocReader.reset();
}

bool MobipocketHtmlBookReader::tagHandler(const HtmlTag &tag) {
	size_t paragraphNumber = myBookReader.model().bookTextModel()->paragraphsNumber();
	if (myBookReader.paragraphIsOpen()) {
		--paragraphNumber;
	}
	myPositionToParagraphMap.push_back(std::pair<size_t,size_t>(tag.Offset, paragraphNumber));
	return HtmlBookReader::tagHandler(tag);
}

MobipocketHtmlBookReader::MobipocketHtmlBookReader(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlBookReader("", model, format, encoding), myFileName(fileName), myTocReader(*this) {
	setBuildTableOfContent(false);
	setProcessPreTag(false);
}

bool MobipocketHtmlBookReader::characterDataHandler(const char *text, int len, bool convert) {
	myTocReader.appendText(text, len);
	return HtmlBookReader::characterDataHandler(text, len, convert);
}

void MobipocketHtmlBookReader::readDocument(ZLInputStream &stream) {
	HtmlBookReader::readDocument(stream);

	shared_ptr<ZLInputStream> fileStream = ZLFile(myFileName).inputStream();
	bool found = false;
	int index = 0;
	if (!fileStream.isNull() && fileStream->open()) {
		char bu[10];
		std::pair<int,int> firstImageLocation = ((MobipocketStream&)stream).imageLocation(0);
		fileStream->seek(firstImageLocation.first, false);
		while ((firstImageLocation.first > 0) && (firstImageLocation.second > 0)) {
			if (firstImageLocation.second > 10) {
				fileStream->read(bu, 10);
				if ((strncmp(bu, "BM", 2) == 0) ||
						(strncmp(bu, "GIF8", 4) == 0) ||
						(strncmp(bu + 6, "JFIF", 4) == 0)) {
					found = true;
					break;
				}
				fileStream->seek(firstImageLocation.second - 10, false);
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

	std::vector<std::pair<size_t,size_t> >::const_iterator jt = myPositionToParagraphMap.begin();
	for (std::set<size_t>::const_iterator it = myFileposReferences.begin(); it != myFileposReferences.end(); ++it) {
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
	const std::map<size_t,std::string> &entries = myTocReader.entries();
	for (std::map<size_t,std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
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
