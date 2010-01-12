/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include "DocBookPlugin.h"
#include "DocBookDescriptionReader.h"
#include "DocBookBookReader.h"
#include "../../library/Book.h"

bool DocBookPlugin::acceptsFile(const std::string &extension) const {
	return extension == "xml";
}

bool DocBookPlugin::readMetaInfo(Book &book) const {
	return DocBookDescriptionReader(book).readMetaInfo(ZLFile(path).inputStream());
}

bool DocBookPlugin::readModel(BookModel &model) const {
	return DocBookBookReader(model).readDocument(ZLFile(book.fileName()).inputStream());
}

const std::string &DocBookPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}
