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

#include <ZLStringUtil.h>
#include <ZLApplication.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GeometricCalculator.h"
#include "GCSaveSceneHandler.h"
#include "../io/SceneNameReader.h"

GCSaveSceneHandler::GCSaveSceneHandler(const std::string &sceneName) : mySceneName(sceneName), myIsUpToDate(false) {
}

void GCSaveSceneHandler::changeFolder(const ZLTreeNode&) {
}

const std::string GCSaveSceneHandler::stateDisplayName() const {
	return mySceneName;
}

const std::vector<ZLTreeNodePtr> &GCSaveSceneHandler::subnodes() const {
	static const std::string Prefix = ZLApplication::ApplicationSubdirectory() + ZLApplication::PathDelimiter;
	static const std::string FolderIcon = Prefix + "folder";
	static const std::string SceneIcon = Prefix + "scene_file";

	if (!myIsUpToDate) {
		myIsUpToDate = true;
		ZLFile(GeometricCalculator::ConfigDirectory()).directory(true);
		shared_ptr<ZLDir> dir = ZLFile(GeometricCalculator::UserCreatedSceneDirectory()).directory(true);
		if (!dir.isNull()) {
			std::vector<std::string> names;
			dir->collectFiles(names, true);
			std::map<std::string,ZLTreeNodePtr> nodeMap;
			for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
				if (ZLFile(*it).extension() == "scn") {
					const std::string displayName = SceneNameReader().readSceneName(ZLFile(dir->itemName(*it)));
					myFileNameBySceneName[displayName] = *it;
					myFileNames.insert(*it);
					ZLTreeNodePtr node = new ZLTreeNode(*it, displayName, SceneIcon, false);
					nodeMap.insert(std::pair<std::string,ZLTreeNodePtr>(displayName, node));
				}
			}
			for (std::map<std::string,ZLTreeNodePtr>::const_iterator jt = nodeMap.begin(); jt != nodeMap.end(); ++jt) {
				mySubnodes.push_back(jt->second);
			}
		}
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

const std::string GCSaveSceneHandler::sceneName() const {
	return mySceneName;
}

const std::string GCSaveSceneHandler::fileName() const {
	std::string shortName;
	std::map<std::string,std::string>::const_iterator it = myFileNameBySceneName.find(sceneName());
	if (it != myFileNameBySceneName.end()) {
		shortName = it->second;
	} else {
		for (int i = 0; i < 10000; ++i) {
			shortName = "scene";
			ZLStringUtil::appendNumber(shortName, i);
			shortName += ".scn";
			if (myFileNames.find(shortName) == myFileNames.end()) {
				break;
			}
		}
	}
	return GeometricCalculator::UserCreatedSceneDirectory() + ZLApplication::PathDelimiter + shortName;
}
