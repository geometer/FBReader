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

#include <ZLStringUtil.h>
#include <ZLApplication.h>
#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLDialogManager.h>

#include "GeometricCalculator.h"
#include "GCSaveSceneHandler.h"

GCSaveSceneHandler::GCSaveSceneHandler(const std::string &sceneName) : mySceneName(sceneName), myIsUpToDate(false) {
}

void GCSaveSceneHandler::changeFolder(const ZLTreeNode&) {
}

const std::string GCSaveSceneHandler::stateDisplayName() const {
	return mySceneName;
}

const std::vector<ZLTreeNodePtr> &GCSaveSceneHandler::subnodes() const {
	if (!myIsUpToDate) {
		collectScenes(UserDirectory(true));
		const std::vector<ZLTreeNodePtr> &nodes = GCSceneHandler::subnodes();
		for (std::vector<ZLTreeNodePtr>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			myFileNames.insert((*it)->id());
			myFileNameBySceneName[(*it)->displayName()] = (*it)->id();
		}
		myIsUpToDate = true;
	}
	return GCSceneHandler::subnodes();
}

int GCSaveSceneHandler::selectedIndex() const {
	return -1;
}

void GCSaveSceneHandler::processNode(const ZLTreeNode &node) {
	mySceneName = node.displayName();
	addUpdateInfo(UPDATE_STATE);
}

bool GCSaveSceneHandler::accept(const std::string &state) {
	if (state.empty()) {
		return false;
	}
	if ((myFileNameBySceneName.find(state) != myFileNameBySceneName.end()) &&
			(ZLDialogManager::instance().questionBox("Save scene", "Scene \"" + state + "\" already exists. Owerwrite?", "Yes", "No") == 1)) {
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
	return UserDirectoryName() + ZLApplication::PathDelimiter + shortName;
}
