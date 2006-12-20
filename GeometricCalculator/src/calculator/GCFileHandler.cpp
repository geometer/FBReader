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

#include <map>

#include <ZLApplication.h>
#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GCFileHandler.h"

const std::string &GCFileHandler::pixmapName(const ZLDir &dir, const std::string &name, bool isFile) const {
	static const std::string prefix = ZLApplication::ApplicationSubdirectory() + ZLApplication::PathDelimiter;
	static const std::string FOLDER_ICON = prefix + "folder";
	static const std::string ZIPFOLDER_ICON = prefix + "zipfolder";
	static const std::string SCENE_FILE_ICON = prefix + "scene_file";
	static const std::string NO_ICON = "";
	if (name.length() == 0) {
		return NO_ICON;
	}
	if (!isFile) {
		return FOLDER_ICON;
	}
	if (ZLFile(name).extension() == "scn") {
		return SCENE_FILE_ICON;
	}
	ZLFile file(dir.itemName(name));
	if (file.isArchive()) {
		return ZIPFOLDER_ICON;
	}
	return NO_ICON;
}

bool GCFileHandler::isAcceptable(const std::string &name) const {
	return ZLFile(name).extension() == "scn";
}

const std::string GCFileHandler::accept(const std::string &name) const {
	myFileName = name;
	return "";
}
