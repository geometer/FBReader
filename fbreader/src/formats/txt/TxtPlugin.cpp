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

#include "TxtPlugin.h"
#include "TxtBookReader.h"
#include "PlainTextFormat.h"
#include "../../description/BookDescription.h"

TxtPlugin::~TxtPlugin() {
}

bool TxtPlugin::providesMetaInfo() const {
	return false;
}

bool TxtPlugin::acceptsFile(const ZLFile &file) const {
	return (file.extension() == "txt") || (file.extension() == "TXT");
}

bool TxtPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);

	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}
	detectEncodingAndLanguage(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}

	return true;
}

bool TxtPlugin::readModel(const BookDescription &description, BookModel &model) const {
	shared_ptr<ZLInputStream> stream = ZLFile(description.fileName()).inputStream();
	if (stream.isNull()) {
		return false;
	}

	PlainTextFormat format(description.fileName());
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	TxtBookReader(model, format, description.encoding()).readDocument(*stream);
	return true;
}

const std::string &TxtPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}

FormatInfoPage *TxtPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	return new PlainTextInfoPage(dialog, fileName, ZLResourceKey("Text"), true);
}
