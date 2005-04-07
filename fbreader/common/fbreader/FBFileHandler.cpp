/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLStringUtil.h>

#include "FBFileHandler.h"
#include "../formats/FormatPlugin.h"

bool FBFileHandler::isFileVisible(const std::string &shortFileName, bool dir) const {
	if (shortFileName.length() == 0) {
		return false;
	}
	if (shortFileName[0] == '.') {
		return dir && (shortFileName == "..");
	}
	return dir || ZLStringUtil::stringEndsWith(shortFileName, ".zip") || (PluginCollection::instance().plugin(shortFileName, false) != 0);
}

std::string FBFileHandler::pixmapName(const std::string &shortFileName, bool dir) const {
	if (dir) {
		return "FBReader/folder";
	} else if (ZLStringUtil::stringEndsWith(shortFileName, "html") || ZLStringUtil::stringEndsWith(shortFileName, ".htm")) {
		return "FBReader/html";
	} else if (ZLStringUtil::stringEndsWith(shortFileName, ".zip")) {
		return "FBReader/zipfolder";
	} else {
		return "FBReader/unknown";
	}
}

void FBFileHandler::accept(const std::string &fullFileName, bool dir) const {
	if (!dir) {
		myDescription = BookDescription::create(fullFileName);
	}
}
