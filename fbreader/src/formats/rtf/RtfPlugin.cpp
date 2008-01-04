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

#include <ZLTime.h>

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLInputStream.h>

#include "RtfPlugin.h"
#include "RtfDescriptionReader.h"
#include "RtfBookReader.h"
#include "../../description/BookDescription.h"

bool RtfPlugin::providesMetaInfo() const {
	return false;
}

bool RtfPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return (extension == "rtf");
}

bool RtfPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);
	shared_ptr<ZLInputStream> stream = file.inputStream();

	if (stream.isNull()) {
		return false;
	}

	detectEncodingAndLanguage(description, *stream);

	if (!RtfDescriptionReader(description).readDocument(path)) {
		return false;
	}
	
	return true;
}

bool RtfPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return RtfBookReader(model, description.encoding()).readDocument(description.fileName());
}

const std::string &RtfPlugin::iconName() const {
	static const std::string ICON_NAME = "rtf";
	return ICON_NAME;
}

FormatInfoPage *RtfPlugin::createInfoPage(ZLOptionsDialog&, const std::string&) {
	return 0;
}
