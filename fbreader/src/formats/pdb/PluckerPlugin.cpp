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

#include "PdbPlugin.h"
#include "PluckerBookReader.h"
#include "PluckerTextStream.h"
#include "../../description/BookDescription.h"

bool PluckerPlugin::providesMetaInfo() const {
	return false;
}

bool PluckerPlugin::acceptsFile(const ZLFile &file) const {
	return PdbPlugin::fileType(file) == "DataPlkr";
}

bool PluckerPlugin::readDescription(const std::string &path, BookDescription &description) const {
	ZLFile file(path);

	shared_ptr<ZLInputStream> stream = new PluckerTextStream(file);
	detectEncodingAndLanguage(description, *stream);
	if (description.encoding().empty()) {
		return false;
	}

	return true;
}

bool PluckerPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return PluckerBookReader(description.fileName(), model, description.encoding()).readDocument();
}

const std::string &PluckerPlugin::iconName() const {
	static const std::string ICON_NAME = "plucker";
	return ICON_NAME;
}
