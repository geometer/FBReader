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

#include "FB2Plugin.h"
#include "FB2DescriptionReader.h"
#include "FB2BookReader.h"
#include "../../description/BookDescription.h"

bool FB2Plugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "fb2";
}

bool FB2Plugin::readDescription(const std::string &path, BookDescription &description) const {
	return FB2DescriptionReader(description).readDescription(path);
}

static const std::string AUTO = "auto";

bool FB2Plugin::readModel(const BookDescription &description, BookModel &model) const {
	// this code fixes incorrect config entry created by fbreader of version <= 0.6.1
	// makes no sense if old fbreader was not used
	if (description.encoding() != AUTO) {
		BookInfo(description.fileName()).EncodingOption.setValue(AUTO);
	}

	return FB2BookReader(model).readBook(description.fileName());
}

const std::string &FB2Plugin::iconName() const {
	static const std::string ICON_NAME = "fb2";
	return ICON_NAME;
}
