/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include "ZLOpenFileDialog.h"

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLDir.h"
#include "../util/ZLStringUtil.h"

static const std::string DefaultDirectory = "~";

ZLStringOption ZLOpenFileDialog::DirectoryOption("OpenFileDialog", "Directory", DefaultDirectory);
	
ZLOpenFileDialog::ZLOpenFileDialog(const ZLTreeHandler &handler) {
	shared_ptr<ZLDir> dir = ZLFile(DirectoryOption.value()).directory();
	if (dir.isNull()) {
		dir = ZLFile(DefaultDirectory).directory();
	}
	myState = new ZLDirTreeState(handler, dir);
}

ZLOpenFileDialog::~ZLOpenFileDialog() {
	DirectoryOption.setValue(state()->name());
}

void ZLOpenFileDialog::runNode(const ZLTreeNodePtr node) {
	ZLTreeStatePtr newState = myState->change(node);
	if (!newState->isLeaf()) {
		std::string selectedName = node->relativeName(myState);
		myState = newState;
		update(selectedName);
	} else {
		newState->handler().accept(*newState);
		exitDialog();
	}
}

ZLTreeNode::ZLTreeNode(const std::string &name, bool isFile, const std::string &pixmapName) : myName(name), myIsFile(isFile), myPixmapName(pixmapName) {
}

ZLTreeNode::~ZLTreeNode() {
}

const std::string &ZLTreeNode::name() const {
	return myName;
}

const std::string &ZLTreeNode::pixmapName() const {
	return myPixmapName;
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

void ZLDirTreeState::addSubnode(const std::string &name, bool isFile) const {
	const std::string &pixmapName = handler().pixmapName(*myDir, name, isFile);
	if (!pixmapName.empty()) {
		mySubnodes.push_back(new ZLTreeNode(name, isFile, pixmapName));
	}
}

const std::vector<ZLTreeNodePtr> &ZLDirTreeState::subnodes() const {
	if (!myIsUpToDate) {
		if (myDir->name() != "/") {
			addSubnode("..", false);
		}
		std::vector<std::string> names;
		myDir->collectSubDirs(names, true);
		std::sort(names.begin(), names.end());
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
			addSubnode(*it, false);
		}
		names.clear();
		myDir->collectFiles(names, true);
		std::sort(names.begin(), names.end());
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
			addSubnode(*it, true);
		}
		myIsUpToDate = true;
	}
	return mySubnodes;
}

ZLTreeStatePtr ZLDirTreeState::change(const ZLTreeNodePtr node) {
	mySubnodes.clear();
	myIsUpToDate = false;
	if (!node->isFile() || ZLFile(node->name()).isArchive()) {
		return new ZLDirTreeState(handler(), ZLFile(myDir->itemName(node->name())).directory());
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
