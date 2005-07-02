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

#include <algorithm>

#include "ZLOpenFileDialog.h"

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLFSDir.h"
#include "../filesystem/ZLZipDir.h"
#include "../util/ZLStringUtil.h"

ZLStringOption ZLOpenFileDialog::DirectoryOption("OpenFileDialog", "Directory", "~");
	
ZLOpenFileDialog::ZLOpenFileDialog(const ZLTreeHandler &handler) {
	std::string dirName =	DirectoryOption.value();
	// TODO: replace this code
	shared_ptr<ZLDir> dir;
	if (ZLStringUtil::stringEndsWith(dirName, ".zip")) {
		dir = new ZLZipDir(dirName);
	} else {
		dir = ZLFSManager::instance().createDirectory(dirName);
	}
	myCurrentDir = new ZLDirTreeState(handler, dir);
}

ZLOpenFileDialog::~ZLOpenFileDialog() {
	DirectoryOption.setValue(myCurrentDir->name());
}

void ZLOpenFileDialog::runNode(const ZLTreeNodePtr node) {
	ZLTreeStatePtr newState = myCurrentDir->change(node);
	if (!newState->isLeaf()) {
		std::string selectedName = node->relativeName(myCurrentDir);
		myCurrentDir = newState;
		update(selectedName);
	} else {
		newState->handler().accept(newState);
		exitDialog();
	}
}

const std::string &ZLOpenFileDialog::pixmapName(const ZLTreeNodePtr node) const {
	return myCurrentDir->handler().pixmapName(node);
}

ZLTreeNode::ZLTreeNode(const std::string &name, bool isFile) : myName(name), myIsFile(isFile) {
}

ZLTreeNode::~ZLTreeNode() {
}

const std::string &ZLTreeNode::name() const {
	return myName;
}

bool ZLTreeNode::isFile() const {
	return myIsFile;
}

std::string ZLTreeNode::relativeName(const ZLTreeStatePtr state) const {
	return (myName == "..") ? state->shortName() : "..";
}

ZLTreeState::ZLTreeState(const ZLTreeHandler &handler) : myHandler(handler) {
}

ZLTreeState::~ZLTreeState() {
}

ZLDirTreeState::ZLDirTreeState(const ZLTreeHandler &handler, shared_ptr<ZLDir> dir) : ZLTreeState(handler), myDir(dir), myIsUpToDate(false) {
}

ZLDirTreeState::~ZLDirTreeState() {
}

const std::string &ZLDirTreeState::name() const {
	return myDir->name();
}

const std::string ZLDirTreeState::shortName() const {
	return myDir->shortName();
}

const std::vector<ZLTreeNodePtr> &ZLDirTreeState::subnodes() const {
	if (!myIsUpToDate) {
		if (myDir->name() != "/") {
			ZLTreeNodePtr node = new ZLTreeNode("..", false);
			if (handler().isNodeVisible(node)) {
				mySubnodes.push_back(node);
			}
		}
		std::vector<std::string> names;
		myDir->collectSubDirs(names, true);
		std::sort(names.begin(), names.end());
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
			ZLTreeNodePtr node = new ZLTreeNode(*it, false);
			if (handler().isNodeVisible(node)) {
				mySubnodes.push_back(node);
			}
		}
		names.clear();
		myDir->collectFiles(names, true);
		std::sort(names.begin(), names.end());
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
			ZLTreeNodePtr node = new ZLTreeNode(*it, true);
			if (handler().isNodeVisible(node)) {
				mySubnodes.push_back(node);
			}
		}
		myIsUpToDate = true;
	}
	return mySubnodes;
}

ZLTreeStatePtr ZLDirTreeState::change(const ZLTreeNodePtr node) {
	mySubnodes.clear();
	myIsUpToDate = false;
	if (!node->isFile()) {
		return new ZLDirTreeState(handler(), ZLFSManager::instance().createDirectory(myDir->itemName(node->name())));
	} else if (ZLStringUtil::stringEndsWith(node->name(), ".zip")) {
		return new ZLDirTreeState(handler(), new ZLZipDir(myDir->itemName(node->name())));
	} else {
		return new ZLFileTreeState(handler(), myDir->itemName(node->name()));
	}
}

bool ZLDirTreeState::isLeaf() const {
	return false;
}

ZLFileTreeState::ZLFileTreeState(const ZLTreeHandler &handler, const std::string &name) : ZLTreeState(handler), myFileName(name) {
}

ZLFileTreeState::~ZLFileTreeState() {
}

const std::string &ZLFileTreeState::name() const {
	return myFileName;
}

const std::string ZLFileTreeState::shortName() const {
	return myFileName;
}

static std::vector<ZLTreeNodePtr> EMPTY_NODE_LIST;

const std::vector<ZLTreeNodePtr> &ZLFileTreeState::subnodes() const {
	return EMPTY_NODE_LIST;
}

ZLTreeStatePtr ZLFileTreeState::change(const ZLTreeNodePtr) {
	return 0;
}

bool ZLFileTreeState::isLeaf() const {
	return true;
}
