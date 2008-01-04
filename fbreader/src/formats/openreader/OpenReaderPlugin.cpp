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
#include <ZLStringUtil.h>
#include <ZLDir.h>

#include "OpenReaderPlugin.h"
#include "ORDescriptionReader.h"
#include "ORBookReader.h"

OpenReaderPlugin::~OpenReaderPlugin() {
}

bool OpenReaderPlugin::providesMetaInfo() const {
	return true;
}

bool OpenReaderPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "orb";
}

bool OpenReaderPlugin::readDescription(const std::string &path, BookDescription &description) const {
	return ORDescriptionReader(description).readDescription(path);
}

bool OpenReaderPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return ORBookReader(model).readBook(description.fileName());
}

const std::string &OpenReaderPlugin::iconName() const {
	static const std::string ICON_NAME = "openreader";
	return ICON_NAME;
}
