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

#include <ZLApplication.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "GCOpenSceneHandler.h"
#include "GeometricCalculator.h"

GCOpenSceneHandler::GCOpenSceneHandler() :
	FolderOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "OpenSceneDialog", "Folder", SamplesFolderName) {
}

shared_ptr<ZLDir> GCOpenSceneHandler::currentDirectory() const {
	if (stateDisplayName() == SamplesFolderName) {
		return SamplesDirectory();
	} else {
		return UserDirectory(false);
	}
}

void GCOpenSceneHandler::changeFolder(const ZLTreeNode &node) {
	FolderOption.setValue(node.id());
	resetSubnodesList();
}

const std::string GCOpenSceneHandler::stateDisplayName() const {
	return FolderOption.value();
}

const std::vector<ZLTreeNodePtr> &GCOpenSceneHandler::subnodes() const {
	if (!isUpToDate()) {
		collectSubnodes((stateDisplayName() == SamplesFolderName) ? UserFolderName : SamplesFolderName, currentDirectory());
	}
	return GCSceneHandler::subnodes();
}

std::string GCOpenSceneHandler::relativeId(const ZLTreeNode &node) const {
	return (node.id() == SamplesFolderName) ? UserFolderName : SamplesFolderName;
}


bool GCOpenSceneHandler::accept(const ZLTreeNode &node) const {
	shared_ptr<ZLDir> dir = currentDirectory();
	myFileName = (dir.isNull()) ? node.id() : dir->itemName(node.id());
	return true;
}

const std::string &GCOpenSceneHandler::fileName() const {
	return myFileName;
}
