/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLDir.h>

#include "FBFileHandler.h"
#include "../formats/FormatPlugin.h"

const std::string &FBFileHandler::pixmapName(const ZLDir &dir, const std::string &name, bool isFile) const {
	static const std::string FOLDER_ICON = ImageDirectory + "/folder";
	static const std::string ZIPFOLDER_ICON = ImageDirectory + "/zipfolder";
	static const std::string NO_ICON = "";
	if (name.length() == 0) {
		return NO_ICON;
	}
	if ((name[0] == '.') && (name != "..")) {
		return NO_ICON;
	}
	if (!isFile) {
		return FOLDER_ICON;
	}
	ZLFile file(dir.itemName(name));
	if (file.isArchive()) {
		return ZIPFOLDER_ICON;
	}
	FormatPlugin *plugin = PluginCollection::instance().plugin(file, false);
	return (plugin != 0) ? (ImageDirectory + "/" + plugin->iconName()) : NO_ICON;
}

void FBFileHandler::accept(const ZLTreeState &state) const {
	myDescription = BookDescription::create(state.name());
}
