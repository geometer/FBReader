/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLDialogManager.h"
#include "ZLSelectionDialog.h"

ZLTreeHandler::ZLTreeHandler() : myUpdateInfo(UPDATE_ALL) {
}

ZLTreeHandler::~ZLTreeHandler() {
}

ZLSelectionDialog::ZLSelectionDialog(ZLTreeHandler &handler) : myHandler(handler) {
}

ZLSelectionDialog::~ZLSelectionDialog() {
}

void ZLSelectionDialog::update() {
	ZLTreeHandler::UpdateType info = handler().updateInfo();
	if (info & ZLTreeHandler::UPDATE_STATE) {
		updateStateLine();
	}
	if (info & ZLTreeHandler::UPDATE_LIST) {
		updateList();
	}
	if (info & ZLTreeHandler::UPDATE_SELECTION) {
		updateSelection();
	}
	myHandler.resetUpdateInfo();
}

void ZLSelectionDialog::updateSelection() {
	const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
	if (nodes.empty()) {
		return;
	}

	int index = handler().selectedIndex();
	if ((index < 0) || (index >= (int)nodes.size())) {
		if (handler().isOpenHandler()) {
			index = 0;
		} else {
			return;
		}
	}
	selectItem(index);
}

void ZLSelectionDialog::runNode(const ZLTreeNodePtr node) {
	if (node.isNull()) {
		return;
	}

	if (node->isFolder()) {
		myHandler.changeFolder(*node);
		update();
	} else if (myHandler.isOpenHandler()) {
		if (((ZLTreeOpenHandler&)myHandler).accept(*node)) {
			exitDialog();
		} else {
			update();
		}
	} else {
		((ZLTreeSaveHandler&)myHandler).processNode(*node);
		update();
	}
}

void ZLSelectionDialog::runState(const std::string &state) {
	if (!myHandler.isOpenHandler()) {
		if (((ZLTreeSaveHandler&)myHandler).accept(state)) {
			exitDialog();
		}
	}
}

ZLTreeNode::ZLTreeNode(const std::string &id, const std::string &displayName, const std::string &pixmapName, bool isFolder) : myId(id), myDisplayName(displayName), myPixmapName(pixmapName), myIsFolder(isFolder) {
}

bool ZLTreeNode::isFolder() const {
	return myIsFolder;
}
