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

#include <ZLDialogManager.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GeometricCalculator.h"
#include "GCRemoveSceneHandler.h"

GCRemoveSceneHandler::GCRemoveSceneHandler() : myIsUpToDate(false), mySelectedIndex(0) {
}

void GCRemoveSceneHandler::changeFolder(const ZLTreeNode&) {
}

const std::string GCRemoveSceneHandler::stateDisplayName() const {
	return UserFolderName;
}

const std::vector<ZLTreeNodePtr> &GCRemoveSceneHandler::subnodes() const {
	if (!myIsUpToDate) {
		collectScenes(UserDirectory(false));
		myIsUpToDate = true;
	}
	return GCSceneHandler::subnodes();
}

int GCRemoveSceneHandler::selectedIndex() const {
	return mySelectedIndex;
}

bool GCRemoveSceneHandler::accept(const ZLTreeNode &node) {
	static const std::string title = "Remove Scene";
	mySelectedIndex = 0;
	const std::string &nodeId = node.id();
	const std::vector<ZLTreeNodePtr> &subnodes = this->subnodes();
	for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		if ((*it)->id() == nodeId) {
			break;
		}
		++mySelectedIndex;
	}
	if (ZLDialogManager::instance().questionBox(title, "Remove Scene \"" + node.displayName() + "\"?", "&Yes", "&No") == 0) {
		if (!ZLFile(UserDirectoryName() + ZLApplication::FileNameDelimiter + node.id()).remove()) {
			ZLDialogManager::instance().errorBox(title, "Couldn't Remove Scene");
		} else {
			resetSubnodesList();
			myIsUpToDate = false;
			--mySelectedIndex;
			addUpdateInfo(UPDATE_LIST);
			addUpdateInfo(UPDATE_SELECTION);
		}
	}
	return false;
}
