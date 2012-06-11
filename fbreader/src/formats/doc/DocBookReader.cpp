/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <iostream>
#include <vector>
#include <string>

#include <ZLInputStream.h>
#include <ZLLogger.h>
#include <ZLBase64EncodedImage.h>
#include <ZLFile.h>
#include <ZLStringUtil.h>

#include "DocBookReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

#include "OleStorage.h"
#include "OleMainStream.h"

DocBookReader::DocBookReader(BookModel &model, const std::string &encoding) :
	OleStreamReader(encoding),
	myModelReader(model) {
	myFieldReading = false;
	myHyperlinkInserted = false;
}

bool DocBookReader::readBook() {
	const ZLFile &file = myModelReader.model().book()->file();
	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}
	return readDocument(stream, file.size());
}

bool DocBookReader::readDocument(shared_ptr<ZLInputStream> inputStream, size_t streamSize) {
	static const std::string WORD_DOCUMENT = "WordDocument";

	if (inputStream.isNull() || !inputStream->open()) {
		return false;
	}
	myModelReader.setMainTextModel();
	myModelReader.pushKind(REGULAR);
	myModelReader.beginParagraph();

	shared_ptr<OleStorage> storage = new OleStorage;

	if (!storage->init(inputStream, streamSize)) {
		ZLLogger::Instance().println("DocBookReader", "Broken OLE file!");
		return false;
	}

	const std::vector<OleEntry>& entries = storage->getEntries();
	for (size_t i = 0; i < entries.size(); ++i) {
		const OleEntry& entry = entries.at(i);
		ZLLogger::Instance().println("DocBookReader", "ole file named... " + entry.name);
		if (entry.type != OleEntry::STREAM || entry.name != WORD_DOCUMENT) {
			ZLLogger::Instance().println("DocBookReader", "is not ole stream");
			continue;
		}

		OleMainStream oleStream(storage, entry, inputStream);
		bool result = readStream(oleStream);
		if (!result) {
			return false;
		}
	}

	myModelReader.insertEndOfTextParagraph();
	return true;
}

//void DocBookReader::parapgraphHandler(std::string paragraph) {
//	myModelReader.beginParagraph();
//	myModelReader.addData(paragraph);
//	myModelReader.endParagraph();
//}

void DocBookReader::handleChar(ZLUnicodeUtil::Ucs2Char ucs2char) {
	if (myFieldReading) {
		myFieldInfoBuffer.push_back(ucs2char);
		return;
	}
	std::string utf8String;
	ZLUnicodeUtil::Ucs2String ucs2String;
	ucs2String.push_back(ucs2char);
	ZLUnicodeUtil::ucs2ToUtf8(utf8String, ucs2String);
	if (!myModelReader.paragraphIsOpen()) {
		myModelReader.beginParagraph();
	}
	myModelReader.addData(utf8String);
}

void DocBookReader::handleHardLinebreak() {
	if (myModelReader.paragraphIsOpen()) {
		myModelReader.endParagraph();
	}
	myModelReader.beginParagraph();
	if (!myCurStyleEntry.isNull()) {
		myModelReader.addControl(*myCurStyleEntry);
	}
	for (size_t i = 0; i < myKindStack.size(); ++i) {
		myModelReader.addControl(myKindStack.at(i), true);
	}
}

void DocBookReader::handleParagraphEnd() {
	if (myModelReader.paragraphIsOpen()) {
		myModelReader.endParagraph();
	}
	myModelReader.beginParagraph();
	myKindStack.clear();
	myCurStyleEntry = 0;
}

void DocBookReader::handlePageBreak() {
	if (myModelReader.paragraphIsOpen()) {
		myModelReader.endParagraph();
	}
	myKindStack.clear();
	myCurStyleEntry = 0;
	myModelReader.insertEndOfSectionParagraph();
	myModelReader.beginParagraph();
}

void DocBookReader::handleTableSeparator() {
	handleChar(SPACE);
	handleChar(VERTICAL_LINE);
	handleChar(SPACE);
}

void DocBookReader::handleTableEndRow() {
	handleParagraphEnd();
}

void DocBookReader::handleFootNoteMark() {
	//TODO implement
}

void DocBookReader::handleStartField() {
	if (myFieldReading) { //for nested fields
		handleEndField();
	}
	myFieldReading = true;
}

void DocBookReader::handleSeparatorField() {
	static const std::string HYPERLINK = "HYPERLINK";
//	static const std::string PAGE = "PAGE";
//	static const std::string PAGEREF = "PAGEREF";
//	static const std::string SHAPE = "SHAPE";
	static const std::string SPACE_DELIMETER = " ";
	static const std::string LOCAL_LINK = "\\l";
	myFieldReading = false;
	std::string utf8String;
	ZLUnicodeUtil::ucs2ToUtf8(utf8String, myFieldInfoBuffer);
	myFieldInfoBuffer.clear();
	ZLStringUtil::stripWhiteSpaces(utf8String);
	if (utf8String.empty()) {
		return;
	}
	std::vector<std::string> result;
	ZLStringUtil::split(utf8String, result, SPACE_DELIMETER);

	if (result.size() >= 2 && result.at(0) == HYPERLINK && result.size() > 1) {
		std::string link = result.at(1);
		if (link == LOCAL_LINK) {
			return; //TODO implement local links
		}
		myModelReader.addHyperlinkControl(EXTERNAL_HYPERLINK, link);
		myHyperlinkInserted = true;
	} else {
		myFieldReading = true;
	}

}

void DocBookReader::handleEndField() {
	//for case if there's no Separator Field
	myFieldReading = false;
	myFieldInfoBuffer.clear();

	if (myHyperlinkInserted) {
		myModelReader.addControl(EXTERNAL_HYPERLINK, false);
		myHyperlinkInserted = false;
	}
}

void DocBookReader::handleStartOfHeading() {
	//heading can be, for example, a picture
	//TODO implement
}

void DocBookReader::handleOtherControlChar(ZLUnicodeUtil::Ucs2Char ucs2char) {
	if (ucs2char == WORD_SHORT_DEFIS) {
		handleChar(SHORT_DEFIS);
	} else if (ucs2char == WORD_SOFT_HYPHEN) {
		//skip
	} else if (ucs2char == WORD_HORIZONTAL_TAB) {
		handleChar(ucs2char);
	} else {
//		myTextBuffer.clear();
	}
}

void DocBookReader::handleFontStyle(unsigned int fontStyle) {
	while (!myKindStack.empty()) {
		myModelReader.addControl(myKindStack.back(), false);
		myKindStack.pop_back();
	}
//	while (!myModelReader.isKindStackEmpty()) {
//		myModelReader.popKind();
//	}
	if (fontStyle & OleMainStream::CharInfo::BOLD) {
		myKindStack.push_back(BOLD);
	}
	if (fontStyle & OleMainStream::CharInfo::ITALIC) {
		myKindStack.push_back(ITALIC);
	}
	for (size_t i = 0; i < myKindStack.size(); ++i) {
		//myModelReader.pushKind(myKindStack.at(i));
		myModelReader.addControl(myKindStack.at(i), true);
	}
}

void DocBookReader::handleParagraphStyle(const OleMainStream::StyleInfo &styleInfo) {
	shared_ptr<ZLTextStyleEntry> entry = new ZLTextStyleEntry();

	if (styleInfo.alignment == OleMainStream::StyleInfo::LEFT) {
		entry->setAlignmentType(ALIGN_LEFT);
	} else if (styleInfo.alignment == OleMainStream::StyleInfo::CENTER) {
		entry->setAlignmentType(ALIGN_CENTER);
	} else if (styleInfo.alignment == OleMainStream::StyleInfo::RIGHT) {
		entry->setAlignmentType(ALIGN_RIGHT);
	} else if (styleInfo.alignment == OleMainStream::StyleInfo::JUSTIFY) {
		entry->setAlignmentType(ALIGN_JUSTIFY);
	}

	if (styleInfo.istd == OleMainStream::H1) {
		entry->setFontSizeMag(3);
	} else if (styleInfo.istd == OleMainStream::H2) {
		entry->setFontSizeMag(2);
	} else if (styleInfo.istd == OleMainStream::H3) {
		entry->setFontSizeMag(1);
	}


	myCurStyleEntry = entry;
	myModelReader.addControl(*myCurStyleEntry);

	handleFontStyle(styleInfo.fontStyle);
}

