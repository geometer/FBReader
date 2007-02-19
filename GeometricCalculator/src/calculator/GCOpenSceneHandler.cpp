/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLApplication.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GCOpenSceneHandler.h"
#include "GeometricCalculator.h"
#include "../io/SceneSetNameReader.h"

GCOpenSceneHandler::GCOpenSceneHandler() :
	FolderOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "OpenSceneDialog", "Folder", AllScenesFolderName), myIsUpToDate(false), mySelectedIndex(0) {
	const std::string &value = FolderOption.value();
	if ((value == AllScenesFolderName) || (value == UserFolderName)) {
		myStateDisplayName = value;
	} else {
		myStateDisplayName = SceneSetNameReader().readSetName(ZLFile(value));
		if (myStateDisplayName.empty()) {
			myStateDisplayName = AllScenesFolderName;
			FolderOption.setValue(AllScenesFolderName);
		}
	}
}

shared_ptr<ZLDir> GCOpenSceneHandler::currentDirectory() const {
	const std::string &value = FolderOption.value();
	if (value == UserFolderName) {
		return UserDirectory(false);
	} else {
		return ZLFile(value).directory(false);
	}
}

void GCOpenSceneHandler::changeFolder(const ZLTreeNode &node) {
	const std::string selectedId = FolderOption.value();
	FolderOption.setValue(node.id());
	resetSubnodesList();
	myIsUpToDate = false;
	mySelectedIndex = 0;
	if (node.id() == AllScenesFolderName) {
		int index = 0;
		const std::vector<ZLTreeNodePtr> &subnodes = this->subnodes();
		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
			if ((*it)->id() == selectedId) {
				mySelectedIndex = index;
				break;
			}
			++index;
		}
		myStateDisplayName = AllScenesFolderName;
	} else {
		myStateDisplayName = node.displayName();
	}
	addUpdateInfo(UPDATE_ALL);
}

const std::string GCOpenSceneHandler::stateDisplayName() const {
	return myStateDisplayName;
}

const std::vector<ZLTreeNodePtr> &GCOpenSceneHandler::subnodes() const {
	if (!myIsUpToDate) {
		if (FolderOption.value() == AllScenesFolderName) {
			addFolderSubnode(UserFolderName, UserFolderName);
			collectSceneArchives(SamplesDirectory());
		} else {
			addFolderSubnode(AllScenesFolderName, "..");
			collectScenes(currentDirectory());
		}
		myIsUpToDate = true;
	}
	return GCSceneHandler::subnodes();
}

int GCOpenSceneHandler::selectedIndex() const {
	return mySelectedIndex;
}

bool GCOpenSceneHandler::accept(const ZLTreeNode &node) {
	shared_ptr<ZLDir> dir = currentDirectory();
	myFileName = (dir.isNull()) ? node.id() : dir->itemPath(node.id());
	return true;
}

const std::string &GCOpenSceneHandler::fileName() const {
	return myFileName;
}
