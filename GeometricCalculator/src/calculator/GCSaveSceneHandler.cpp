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
#include <ZLFile.h>
#include <ZLDir.h>

#include "GCSaveSceneHandler.h"
#include "../io/SceneNameReader.h"

GCSaveSceneHandler::GCSaveSceneHandler(const std::string &sceneName) : mySceneName(sceneName), myIsUpToDate(false) {
}

shared_ptr<ZLDir> GCSaveSceneHandler::currentDirectory() const {
	static const std::string home = ZLApplication::HomeDirectory + ZLApplication::PathDelimiter + "." + ZLApplication::ApplicationName();
	ZLFile(home).directory(true);
	return ZLFile(home + ZLApplication::PathDelimiter + "scenes").directory(true);
}

void GCSaveSceneHandler::changeFolder(const ZLTreeNode&) {
	mySubnodes.clear();
	myIsUpToDate = false;
}

const std::string GCSaveSceneHandler::stateDisplayName() const {
	return mySceneName;
}

const std::vector<ZLTreeNodePtr> &GCSaveSceneHandler::subnodes() const {
	static const std::string Prefix = ZLApplication::ApplicationSubdirectory() + ZLApplication::PathDelimiter;
	static const std::string FolderIcon = Prefix + "folder";
	static const std::string SceneIcon = Prefix + "scene_file";

	if (!myIsUpToDate) {
		std::vector<std::string> names;
		shared_ptr<ZLDir> dir = currentDirectory();
		if (!dir.isNull()) {
			dir->collectFiles(names, true);
			std::map<std::string,ZLTreeNodePtr> nodeMap;
			for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
				if (ZLFile(*it).extension() == "scn") {
					std::string displayName = SceneNameReader().readSceneName(ZLFile(dir->itemName(*it)));
					ZLTreeNodePtr node = new ZLTreeNode(*it, displayName, SceneIcon, false);
					nodeMap.insert(std::pair<std::string,ZLTreeNodePtr>(displayName, node));
				}
			}
			for (std::map<std::string,ZLTreeNodePtr>::const_iterator jt = nodeMap.begin(); jt != nodeMap.end(); ++jt) {
				mySubnodes.push_back(jt->second);
			}
		}
		myIsUpToDate = true;
	}
	return mySubnodes;
}

std::string GCSaveSceneHandler::relativeId(const ZLTreeNode &node) const {
	return node.id();
}

void GCSaveSceneHandler::processNode(const ZLTreeNode &node) const {
	mySceneName = node.displayName();
}

bool GCSaveSceneHandler::accept(const std::string &state) const {
	if (state.empty()) {
		return false;
	}
	mySceneName = state;
	return true;
}
