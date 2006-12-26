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

#include "GCOpenSceneHandler.h"
#include "../io/SceneNameReader.h"

static const std::string SAMPLES_FOLDER = "Samples";
static const std::string USER_FOLDER = "User Created";

GCOpenSceneHandler::GCOpenSceneHandler() :
	FolderOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "OpenSceneDialog", "Folder", SAMPLES_FOLDER), myIsUpToDate(false) {
}

shared_ptr<ZLDir> GCOpenSceneHandler::currentDirectory() const {
	if (stateDisplayName() == SAMPLES_FOLDER) {
		return ZLFile(ZLApplication::ApplicationDirectory() + ZLApplication::PathDelimiter + "samples.tar").directory();
	} else {
		static const std::string home = ZLApplication::HomeDirectory + ZLApplication::PathDelimiter + "." + ZLApplication::ApplicationName();
		ZLFile(home).directory(true);
		return ZLFile(home + ZLApplication::PathDelimiter + "scenes").directory(true);
	}
}

void GCOpenSceneHandler::changeFolder(const ZLTreeNode &node) {
	FolderOption.setValue(node.id());
	mySubnodes.clear();
	myIsUpToDate = false;
}

const std::string GCOpenSceneHandler::stateDisplayName() const {
	return FolderOption.value();
}

static const std::string &opposite(const std::string &nodeId) {
	return (nodeId == SAMPLES_FOLDER) ? USER_FOLDER : SAMPLES_FOLDER;
}

const std::vector<ZLTreeNodePtr> &GCOpenSceneHandler::subnodes() const {
	static const std::string Prefix = ZLApplication::ApplicationSubdirectory() + ZLApplication::PathDelimiter;
	static const std::string FolderIcon = Prefix + "folder";
	static const std::string SceneIcon = Prefix + "scene_file";

	if (!myIsUpToDate) {
		const std::string folderName = opposite(stateDisplayName());
		mySubnodes.push_back(new ZLTreeNode(folderName, folderName, FolderIcon, true));
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

std::string GCOpenSceneHandler::relativeId(const ZLTreeNode &node) const {
	return opposite(node.id());
}


bool GCOpenSceneHandler::accept(const ZLTreeNode &node) const {
	shared_ptr<ZLDir> dir = currentDirectory();
	myFileName = (dir.isNull()) ? node.id() : dir->itemName(node.id());
	return true;
}
