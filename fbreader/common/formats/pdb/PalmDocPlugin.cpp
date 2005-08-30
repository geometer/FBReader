/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "PdbPlugin.h"
#include "PalmDocStream.h"
#include "../../description/BookDescription.h"
#include "../txt/TxtBookReader.h"
#include "../txt/PlainTextFormat.h"

bool PalmDocPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::acceptsFile(file, "TEXtREAd");
}

bool PalmDocPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);

	shared_ptr<ZLInputStream> stream = new PalmDocStream(file);
	detectEncoding(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}
	defaultAuthor(description);
	defaultTitle(description, file.name());
	defaultLanguage(description);

	return true;
}

bool PalmDocPlugin::readModel(const BookDescription &description, BookModel &model) const {
	ZLFile file(description.fileName());
	shared_ptr<ZLInputStream> stream = new PalmDocStream(file);

	PlainTextFormat format(description.fileName());
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	TxtBookReader(model, format).readDocument(*stream, description.encoding());
	return true;
}

const std::string &PalmDocPlugin::iconName() const {
	static const std::string ICON_NAME = "FBReader/pdb";
	return ICON_NAME;
}

FormatInfoPage *PalmDocPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	return new PlainTextInfoPage(dialog, fileName, "Text", true);
}
