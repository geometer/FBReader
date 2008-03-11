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

#include "OEBPlugin.h"
#include "OEBDescriptionReader.h"
#include "OEBBookReader.h"
#include "../../description/BookDescription.h"

static const std::string OPF = "opf";
static const std::string OEBZIP = "oebzip";
static const std::string EPUB = "epub";

OEBPlugin::~OEBPlugin() {
}

bool OEBPlugin::providesMetaInfo() const {
	return true;
}

bool OEBPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return (extension == OPF) || (extension == OEBZIP) || (extension == EPUB);
}

std::string OEBPlugin::opfFileName(const std::string &oebFileName) {
	ZLFile oebFile = ZLFile(oebFileName);
	if (oebFile.extension() == OPF) {
		return oebFileName;
	}

	oebFile.forceArchiveType(ZLFile::ZIP);
	shared_ptr<ZLDir> zipDir = oebFile.directory(false);
	if (zipDir.isNull()) {
		return "";
	}
	std::vector<std::string> fileNames;
	zipDir->collectFiles(fileNames, false);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		if (ZLStringUtil::stringEndsWith(*it, ".opf")) {
			return zipDir->itemPath(*it);
		}
	}
	return "";
}

bool OEBPlugin::readDescription(const std::string &path, BookDescription &description) const {
	return OEBDescriptionReader(description).readDescription(opfFileName(path));
}

bool OEBPlugin::readModel(const BookDescription &description, BookModel &model) const {
	return OEBBookReader(model).readBook(opfFileName(description.fileName()));
}

const std::string &OEBPlugin::iconName() const {
	static const std::string ICON_NAME = "oeb";
	return ICON_NAME;
}
