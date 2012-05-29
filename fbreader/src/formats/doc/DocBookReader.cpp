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

#include <iostream>
#include <vector>
#include <string>

#include "DocBookReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

#include "OleStorage.h"
#include "OleStream.h"

const static std::string WORD_DOCUMENT = "WordDocument";

DocBookReader::DocBookReader(BookModel &model, const std::string &encoding) :
	OleStreamReader(encoding), myModelReader(model) {
}

bool DocBookReader::readBook() {
	const ZLFile &file = myModelReader.model().book()->file();
	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}
	return readDocument(stream, file.size());
}

void DocBookReader::parapgraphHandler(std::string paragraph) {
	myModelReader.beginParagraph();
	myModelReader.addData(paragraph);
	myModelReader.endParagraph();
}

bool DocBookReader::readDocument(shared_ptr<ZLInputStream> inputStream,size_t streamSize) {
	if (inputStream.isNull() || !inputStream->open()) {
		return false;
	}
	myModelReader.setMainTextModel();

	shared_ptr<OleStorage> storage = new OleStorage;

	if (!storage->init(inputStream, streamSize)) {
		ZLLogger::Instance().println("DocBookReader", "Broken OLE file!");
		return false;
	}

	const std::vector<OleEntry>& entries = storage->getEntries();
	for (size_t i = 0; i < entries.size(); ++i ) {
		const OleEntry& entry = entries.at(i);
		ZLLogger::Instance().print("DocBookReader", "ole file named... " + entry.name);
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
