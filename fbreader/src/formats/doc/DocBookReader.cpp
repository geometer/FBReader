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

#include <ZLInputStream.h>
#include <ZLLogger.h>
#include <ZLBase64EncodedImage.h>
#include <ZLFile.h>
#include <ZLStringUtil.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>

#include "DocBookReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

#include "OleStorage.h"
#include "OleStream.h"

const static std::string WORD_DOCUMENT = "WordDocument";

DocBookReader::DocBookReader(BookModel &model, const std::string &encoding) :
	OleStreamReader(encoding), myModelReader(model) {
	myTabMode = false;
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

bool DocBookReader::readDocument(shared_ptr<ZLInputStream> inputStream,size_t streamSize) {
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
	for (size_t i = 0; i < entries.size(); ++i ) {
		const OleEntry& entry = entries.at(i);
		ZLLogger::Instance().println("DocBookReader", "ole file named... " + entry.name);
		if (entry.type != OleEntry::STREAM || entry.name != WORD_DOCUMENT) {
			ZLLogger::Instance().println("DocBookReader", "is not ole stream");
			continue;
		}

		OleStream oleStream(storage, entry, inputStream);
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
	handleParagraphEnd();
}

void DocBookReader::handleParagraphEnd() {
	if (myModelReader.paragraphIsOpen()) {
		myModelReader.endParagraph();
	}
	myModelReader.beginParagraph();
}

void DocBookReader::handlePageBreak() {
	//myModelReader.insertEndOfSectionParagraph();
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
		myHyperlinkInserted  = false;
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

