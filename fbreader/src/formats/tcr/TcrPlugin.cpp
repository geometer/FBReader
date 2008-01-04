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

#include <ZLFile.h>
#include <ZLInputStream.h>

#include "TcrPlugin.h"
#include "TcrStream.h"
#include "PPLBookReader.h"
#include "../util/TextFormatDetector.h"
#include "../../description/BookDescription.h"
#include "../txt/TxtBookReader.h"
#include "../html/HtmlBookReader.h"
#include "../txt/PlainTextFormat.h"

bool TcrPlugin::acceptsFile(const ZLFile &file) const {
	return (file.extension() == "tcr") || (file.extension() == "TCR");
}

bool TcrPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);

	shared_ptr<ZLInputStream> stream = new TcrStream(file);
	detectEncodingAndLanguage(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}

	return true;
}

bool TcrPlugin::readModel(const BookDescription &description, BookModel &model) const {
	ZLFile file(description.fileName());
	shared_ptr<ZLInputStream> stream = new TcrStream(file);

	PlainTextFormat format(description.fileName());
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	if (TextFormatDetector().isPPL(*stream)) {
		PPLBookReader(model, description.encoding()).readDocument(*stream);
	} else if (TextFormatDetector().isHtml(*stream)) {
		HtmlBookReader("", model, format, description.encoding()).readDocument(*stream);
	} else {
		TxtBookReader(model, format, description.encoding()).readDocument(*stream);
	}
	return true;
}

const std::string &TcrPlugin::iconName() const {
	static const std::string ICON_NAME = "tcr";
	return ICON_NAME;
}

FormatInfoPage *TcrPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	ZLFile file(fileName);
	shared_ptr<ZLInputStream> stream = new TcrStream(file);
	if (TextFormatDetector().isPPL(*stream)) {
		return 0;
	}
	return new PlainTextInfoPage(dialog, fileName, ZLResourceKey("Text"), !TextFormatDetector().isHtml(*stream));
}
