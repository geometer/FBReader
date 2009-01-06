/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "PdfPlugin.h"
#include "PdfDescriptionReader.h"
#include "PdfBookReader.h"
#include "../../description/BookDescription.h"

bool PdfPlugin::acceptsFile(const ZLFile &file) const {
	return file.extension() == "pdf";
}

bool PdfPlugin::readDescription(const std::string &path, BookDescription &description) const {
	return PdfDescriptionReader(description).readDescription(ZLFile(path).inputStream());
}

bool PdfPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return PdfBookReader(model).readBook(ZLFile(description.fileName()).inputStream());
}

const std::string &PdfPlugin::iconName() const {
	static const std::string ICON_NAME = "unknown";
	return ICON_NAME;
}
