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

#include <map>

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLApplication.h>
#include <ZLibrary.h>

#include "GCSceneHandler.h"
#include "GeometricCalculator.h"
#include "../io/SceneNameReader.h"
#include "../io/SceneSetNameReader.h"

const std::string GCSceneHandler::AllScenesFolderName = "All Scenes";
const std::string GCSceneHandler::UserFolderName = "User Created";

shared_ptr<ZLDir> GCSceneHandler::SamplesDirectory() {
	return ZLFile(ZLApplication::ApplicationDirectory() + ZLibrary::FileNameDelimiter + "samples").directory();
}

const std::string GCSceneHandler::UserDirectoryName() {
	return GeometricCalculator::ConfigDirectory() + ZLibrary::FileNameDelimiter + "scenes";
}

shared_ptr<ZLDir> GCSceneHandler::UserDirectory(bool create) {
	if (create) {
		ZLFile configDirectory(GeometricCalculator::ConfigDirectory());
		configDirectory.directory(true);
	}
	return ZLFile(UserDirectoryName()).directory(create);
}

GCSceneHandler::GCSceneHandler() {
}

void GCSceneHandler::addFolderSubnode(const std::string &id, const std::string &displayName) const {
	static const std::string FolderIcon = "folder";

	mySubnodes.push_back(new ZLTreeNode(id, displayName, FolderIcon, true));
}

void GCSceneHandler::collectScenes(shared_ptr<ZLDir> dir) const {
	static const std::string SceneIcon = "scene_file";

	if (!dir.isNull()) {
		std::vector<std::string> names;
		dir->collectFiles(names, true);
		std::map<std::string,ZLTreeNodePtr> nodeMap;
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (ZLFile(*it).extension() == "scn") {
				std::string displayName = SceneNameReader().readSceneName(ZLFile(dir->itemPath(*it)));
				ZLTreeNodePtr node = new ZLTreeNode(*it, displayName, SceneIcon, false);
				nodeMap.insert(std::pair<std::string,ZLTreeNodePtr>(displayName, node));
			}
		}
		for (std::map<std::string,ZLTreeNodePtr>::const_iterator jt = nodeMap.begin(); jt != nodeMap.end(); ++jt) {
			mySubnodes.push_back(jt->second);
		}
	}
}

void GCSceneHandler::collectSceneArchives(shared_ptr<ZLDir> dir) const {
	static const std::string FolderIcon = "folder";

	if (!dir.isNull()) {
		std::vector<std::string> names;
		dir->collectFiles(names, true);
		std::map<std::string,ZLTreeNodePtr> nodeMap;
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
			if (ZLFile(*it).extension() == "tar") {
				const std::string archiveName = dir->itemPath(*it);
				ZLFile archiveFile(archiveName);
				std::string displayName = SceneSetNameReader().readSetName(archiveFile);
				if (!displayName.empty()) {
					ZLTreeNodePtr node = new ZLTreeNode(archiveName, displayName, FolderIcon, true);
					nodeMap.insert(std::pair<std::string,ZLTreeNodePtr>(displayName, node));
				}
			}
		}
		for (std::map<std::string,ZLTreeNodePtr>::const_iterator jt = nodeMap.begin(); jt != nodeMap.end(); ++jt) {
			mySubnodes.push_back(jt->second);
		}
	}
}

const std::vector<ZLTreeNodePtr> &GCSceneHandler::subnodes() const {
	return mySubnodes;
}

void GCSceneHandler::resetSubnodesList() const {
	mySubnodes.clear();
}
