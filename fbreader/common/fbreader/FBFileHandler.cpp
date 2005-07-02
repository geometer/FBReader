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

bool FBFileHandler::isNodeVisible(const ZLTreeNodePtr node) const {
	const std::string &name = node->name();
	if (name.length() == 0) {
		return false;
	}
	if (name[0] == '.') {
		return !node->isFile() && (name == "..");
	}
	return !node->isFile() || ZLStringUtil::stringEndsWith(name, ".zip") || (PluginCollection::instance().plugin(name, false) != 0);
}

const std::string &FBFileHandler::pixmapName(const ZLTreeNodePtr node) const {
	static const std::string FOLDER_ICON = "FBReader/folder";
	static const std::string ZIPFOLDER_ICON = "FBReader/zipfolder";
	static const std::string UNKNOWN_ICON = "FBReader/unknown";
	if (!node->isFile()) {
		return FOLDER_ICON;
	} else if (ZLStringUtil::stringEndsWith(node->name(), ".zip")) {
		return ZIPFOLDER_ICON;
	} else {
		FormatPlugin *plugin = PluginCollection::instance().plugin(node->name(), false);
		if (plugin != 0) {
			return plugin->iconName();
		}
	}
	return UNKNOWN_ICON;
}

void FBFileHandler::accept(const ZLTreeStatePtr state) const {
	myDescription = BookDescription::create(state->name());
}
