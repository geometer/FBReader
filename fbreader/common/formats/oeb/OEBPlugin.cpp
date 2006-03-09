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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "OEBPlugin.h"
#include "OEBDescriptionReader.h"
#include "OEBBookReader.h"
#include "../../description/BookDescription.h"

bool OEBPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "dummy";
}

bool OEBPlugin::readDescription(const std::string &path, BookDescription &description) const {
	return OEBDescriptionReader(description).readDescription(ZLFile(path).inputStream());
}

bool OEBPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return OEBBookReader(model).readBook(ZLFile(description.fileName()).inputStream());
}

const std::string &OEBPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}
