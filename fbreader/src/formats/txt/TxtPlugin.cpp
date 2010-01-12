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

#include <ZLFile.h>
#include <ZLInputStream.h>

#include "TxtPlugin.h"
#include "TxtBookReader.h"
#include "PlainTextFormat.h"

#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

TxtPlugin::~TxtPlugin() {
}

bool TxtPlugin::providesMetaInfo() const {
	return false;
}

bool TxtPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "txt";
}

bool TxtPlugin::readMetaInfo(Book &book) const {
	ZLFile file(book.filePath());

	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}
	detectEncodingAndLanguage(book, *stream);
	if (book.encoding().empty()) {
		return false;
	}

	return true;
}

bool TxtPlugin::readModel(BookModel &model) const {
	const Book &book = *model.book();
	const std::string &filePath = book.filePath();
	shared_ptr<ZLInputStream> stream = ZLFile(filePath).inputStream();
	if (stream.isNull()) {
		return false;
	}

	PlainTextFormat format(filePath);
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	TxtBookReader(model, format, book.encoding()).readDocument(*stream);
	return true;
}

const std::string &TxtPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}

FormatInfoPage *TxtPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	return new PlainTextInfoPage(dialog, fileName, ZLResourceKey("Text"), true);
}
