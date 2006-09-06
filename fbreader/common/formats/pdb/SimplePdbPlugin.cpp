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

#include <abstract/ZLFile.h>
#include <abstract/ZLInputStream.h>

#include "PdbPlugin.h"
#include "../util/TextFormatDetector.h"
#include "../../description/BookDescription.h"
#include "../txt/TxtBookReader.h"
#include "../html/HtmlBookReader.h"
#include "../txt/PlainTextFormat.h"

bool SimplePdbPlugin::providesMetaInfo() const {
	return false;
}

bool SimplePdbPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);

	shared_ptr<ZLInputStream> stream = createStream(file);
	detectEncodingAndLanguage(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}
	defaultTitle(description, file.name());

	return true;
}

bool SimplePdbPlugin::readModel(const BookDescription &description, BookModel &model) const {
	ZLFile file(description.fileName());
	shared_ptr<ZLInputStream> stream = createStream(file);

	PlainTextFormat format(description.fileName());
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	readDocumentInternal(description.fileName(), model, format, description.encoding(), *stream);
	return true;
}

void SimplePdbPlugin::readDocumentInternal(const std::string&, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	if (TextFormatDetector().isHtml(stream)) {
		HtmlBookReader("", model, format, encoding).readDocument(stream);
	} else {
		TxtBookReader(model, format, encoding).readDocument(stream);
	}
}

FormatInfoPage *SimplePdbPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	ZLFile file(fileName);
	shared_ptr<ZLInputStream> stream = createStream(file);
	return new PlainTextInfoPage(dialog, fileName, "Text", !TextFormatDetector().isHtml(*stream));
}
