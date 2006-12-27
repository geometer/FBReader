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

#include <ZLDialogManager.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GeometricCalculator.h"
#include "GCRemoveSceneHandler.h"

GCRemoveSceneHandler::GCRemoveSceneHandler() {
}

void GCRemoveSceneHandler::changeFolder(const ZLTreeNode&) {
}

const std::string GCRemoveSceneHandler::stateDisplayName() const {
	return UserFolderName;
}

const std::vector<ZLTreeNodePtr> &GCRemoveSceneHandler::subnodes() const {
	if (!isUpToDate()) {
		collectSubnodes("", UserDirectory(false));
	}
	return GCSceneHandler::subnodes();
}

std::string GCRemoveSceneHandler::relativeId(const ZLTreeNode &node) const {
	return node.id();
}


bool GCRemoveSceneHandler::accept(const ZLTreeNode &node) const {
	static const std::string title = "Remove Scene";
	if (ZLDialogManager::instance().questionBox(title, "Remove Scene \"" + node.displayName() + "\"?", "Yes", "No") == 0) {
		if (!ZLFile(UserDirectoryName() + ZLApplication::PathDelimiter + node.id()).remove()) {
			ZLDialogManager::instance().errorBox(title, "Couldn't Remove Scene");
		} else {
			resetSubnodesList();
		}
	}
	return false;
}
