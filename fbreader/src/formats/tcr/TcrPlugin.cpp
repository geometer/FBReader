/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include "TcrPlugin.h"
#include "TcrStream.h"
#include "PPLBookReader.h"
#include "../util/TextFormatDetector.h"
#include "../txt/TxtBookReader.h"
#include "../html/HtmlBookReader.h"
#include "../txt/PlainTextFormat.h"

#include "../../bookmodel/BookModel.h"
#include "../../library/Book.h"

bool TcrPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "tcr";
}

bool TcrPlugin::readMetaInfo(Book &book) const {
	shared_ptr<ZLInputStream> stream = new TcrStream(book.file());
	detectEncodingAndLanguage(book, *stream);
	if (book.encoding().empty()) {
		return false;
	}

	return true;
}

bool TcrPlugin::readLanguageAndEncoding(Book &book) const {
	(void)book;
	return true;
}

bool TcrPlugin::readModel(BookModel &model) const {
	const Book &book = *model.book();
	const ZLFile &file = book.file();

	shared_ptr<ZLInputStream> stream = new TcrStream(file);

	PlainTextFormat format(file);
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	const std::string &encoding = book.encoding();
	if (TextFormatDetector().isPPL(*stream)) {
		PPLBookReader(model, encoding).readDocument(*stream);
	} else if (TextFormatDetector().isHtml(*stream)) {
		HtmlBookReader("", model, format, encoding).readDocument(*stream);
	} else {
		TxtBookReader(model, format, encoding).readDocument(*stream);
	}
	return true;
}

FormatInfoPage *TcrPlugin::createInfoPage(ZLOptionsDialog &dialog, const ZLFile &file) {
	shared_ptr<ZLInputStream> stream = new TcrStream(file);
	if (TextFormatDetector().isPPL(*stream)) {
		return 0;
	}
	return new PlainTextInfoPage(dialog, file, ZLResourceKey("Text"), !TextFormatDetector().isHtml(*stream));
}
