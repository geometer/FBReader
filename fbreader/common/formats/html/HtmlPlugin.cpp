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

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "HtmlPlugin.h"
#include "HtmlDescriptionReader.h"
#include "HtmlBookReader.h"
#include "../txt/PlainTextFormat.h"
#include "../../description/BookDescription.h"

bool HtmlPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return ZLStringUtil::stringEndsWith(extension, "html") || (extension == "htm");
}

bool HtmlPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);
	shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}

	detectEncoding(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}
	HtmlDescriptionReader(description).readDocument(*stream);
	defaultTitle(description, file.name());
	defaultLanguage(description);

	return true;
}

bool HtmlPlugin::readModel(const BookDescription &description, BookModel &model) const {
	std::string fileName = description.fileName();
	shared_ptr<ZLInputStream> stream = ZLFile(fileName).inputStream();
	if (stream.isNull()) {
		return false;
	}

	PlainTextFormat format(fileName);
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	int index0 = fileName.rfind('/');
	int index1 = fileName.rfind(':');
	HtmlBookReader(fileName.substr(0, std::max(index0, index1) + 1), model, format, description.encoding()).readDocument(*stream);

	return true;
}

const std::string &HtmlPlugin::iconName() const {
	static const std::string ICON_NAME = "html";
	return ICON_NAME;
}

FormatInfoPage *HtmlPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	return new PlainTextInfoPage(dialog, fileName, "<PRE>", false);
}
