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

#include <iostream>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLDir.h>

#include "OEBPlugin.h"
#include "OEBDescriptionReader.h"
#include "OEBBookReader.h"
#include "../../description/BookDescription.h"

static const std::string OPF = "opf";
static const std::string OEBZIP = "oebzip";

bool OEBPlugin::acceptsFile(const ZLFile &file) const {
  const std::string &extension = file.extension();
	return (extension == OPF) || (extension == OEBZIP);
}

std::string OEBPlugin::opfFileName(const std::string &oebFileName) const {
	ZLFile oebFile = ZLFile(oebFileName);
  if (oebFile.extension() == OPF) {
		return oebFileName;
	}

	shared_ptr<ZLDir> zipDir = oebFile.directory(false);
	if (zipDir.isNull()) {
		return "";
	}
	std::vector<std::string> fileNames;
	zipDir->collectFiles(fileNames, false);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (ZLStringUtil::stringEndsWith(*it, ".opf")) {
			return zipDir->itemName(*it);
		}
	}
	return "";
}

bool OEBPlugin::readDescription(const std::string &path, BookDescription &description) const {
  return OEBDescriptionReader(description).readDescription(ZLFile(opfFileName(path)).inputStream());
}

bool OEBPlugin::readModel(const BookDescription &description, BookModel &model) const {
  return OEBBookReader(model).readBook(opfFileName(description.fileName()));
}

const std::string &OEBPlugin::iconName() const {
  static const std::string ICON_NAME = "oeb";
  return ICON_NAME;
}
