/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "HtmlPlugin.h"
#include "HtmlDescriptionReader.h"
#include "HtmlBookReader.h"
#include "../EncodingDetector.h"
#include "../../description/BookDescription.h"

bool HtmlPlugin::acceptsFile(const std::string &fileName) const {
	return ZLStringUtil::stringEndsWith(fileName, "html") || ZLStringUtil::stringEndsWith(fileName, ".htm");
}

bool HtmlPlugin::readDescription(const std::string &fileName, BookDescription &description) const {
	ZLInputStream *stream = ZLFSManager::instance().createInputStream(fileName);
	std::string encoding = description.encoding();
	if (encoding.empty()) {
		encoding = EncodingDetector::detect(*stream);
		if (encoding.empty()) {
			delete stream;
			return false;
		}
		WritableBookDescription(description).encoding() = encoding;
	}

	if (description.author() == 0) {
		WritableBookDescription(description).addAuthor("Unknown", "", "Author");
	}

	HtmlDescriptionReader *reader = new HtmlDescriptionReader(description);
	reader->readDocument(*stream, encoding);
	delete reader;

	if (description.title().empty()) {
		int index = fileName.rfind('/');
		std::string title = (index > 0) ? fileName.substr(index + 1) : fileName;
		index = title.rfind('.');
		WritableBookDescription(description).title() = (index > 0) ? title.substr(0, index) : title;
	}

	if (description.language() == "") {
		if (description.encoding() == "US-ASCII") {
			WritableBookDescription(description).language() = "en";
		} else if ((description.encoding() == "KOI8-R") ||
				(description.encoding() == "windows-1251") ||
				(description.encoding() == "ISO-8859-5") ||
				(description.encoding() == "IBM866")) {
			WritableBookDescription(description).language() = "ru";
		}
	}
	delete stream;
	return true;
}

bool HtmlPlugin::readModel(const BookDescription &description, BookModel &model) const {
	HtmlBookReader *reader = new HtmlBookReader(model);
	ZLInputStream *stream = ZLFSManager::instance().createInputStream(description.fileName());
	reader->readDocument(*stream, description.encoding());
	delete stream;
	delete reader;
	return true;
}

const std::string &HtmlPlugin::iconName() const {
	static const std::string ICON_NAME = "FBReader/html";
	return ICON_NAME;
}
