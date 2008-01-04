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
#include <ZLInputStream.h>

#include "PdbPlugin.h"
#include "MobipocketStream.h"
#include "MobipocketHtmlBookReader.h"

bool MobipocketPlugin::acceptsFile(const ZLFile &file) const {
	const std::string type = PdbPlugin::fileType(file);
	return type == "BOOKMOBI";
}

void MobipocketPlugin::readDocumentInternal(const std::string &fileName, BookModel &model, const PlainTextFormat &format, const std::string &encoding, ZLInputStream &stream) const {
	MobipocketHtmlBookReader(fileName, model, format, encoding).readDocument(stream);
}

const std::string &MobipocketPlugin::iconName() const {
	static const std::string ICON_NAME = "mobipocket";
	return ICON_NAME;
}
