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

#ifndef __ZLSELECTIONDIALOG_H__
#define __ZLSELECTIONDIALOG_H__

#include <vector>
#include <string>

#include <shared_ptr.h>

class ZLTreeNode;
typedef shared_ptr<ZLTreeNode> ZLTreeNodePtr;

class ZLTreeHandler {

public:
	enum UpdateType {
		UPDATE_NONE = 0,
		UPDATE_STATE = 1,
		UPDATE_LIST = 2,
		UPDATE_SELECTION = 4,
		UPDATE_ALL = UPDATE_STATE | UPDATE_LIST | UPDATE_SELECTION
	};

protected:
	ZLTreeHandler();
	virtual ~ZLTreeHandler();
	
public:
	virtual bool isOpenHandler() const = 0;

	UpdateType updateInfo() const;
	void resetUpdateInfo();
	virtual const std::string stateDisplayName() const = 0;
	virtual const std::vector<ZLTreeNodePtr> &subnodes() const = 0;
	virtual int selectedIndex() const = 0;
	
	virtual void changeFolder(const ZLTreeNode &node) = 0;

protected:
	void addUpdateInfo(UpdateType info);

private:
	UpdateType myUpdateInfo;
};

class ZLTreeOpenHandler : public ZLTreeHandler {

public:
	virtual bool isOpenHandler() const { return true; }

	virtual bool accept(const ZLTreeNode &node) = 0;
};

class ZLTreeSaveHandler : public ZLTreeHandler {

public:
	virtual bool isOpenHandler() const { return false; }

	virtual void processNode(const ZLTreeNode &node) = 0;
	virtual bool accept(const std::string &state) = 0;
};

class ZLTreeNode {

public:
	ZLTreeNode(const std::string &id, const std::string &displayName, const std::string &pixmapName, bool isFolder);

	const std::string &id() const;
	const std::string &displayName() const;
	const std::string &pixmapName() const;
	bool isFolder() const;

private:
	std::string myId;
	std::string myDisplayName;
	std::string myPixmapName;
	bool myIsFolder;
};

class ZLSelectionDialog {

protected:
	ZLSelectionDialog(ZLTreeHandler &handler);
	virtual ~ZLSelectionDialog();

public:
	virtual bool run() = 0;

protected:
	const ZLTreeHandler &handler() const;
	void runNode(const ZLTreeNodePtr node);
	void runState(const std::string &state);
	virtual void exitDialog() = 0;

	virtual void updateStateLine() = 0;
	virtual void updateList() = 0;
	virtual void selectItem(int index) = 0;

	virtual void update();

private:
	virtual void updateSelection();

private:
	ZLTreeHandler &myHandler;
};

inline ZLTreeHandler::UpdateType ZLTreeHandler::updateInfo() const { return myUpdateInfo; }
inline void ZLTreeHandler::addUpdateInfo(UpdateType info) { myUpdateInfo = (UpdateType)(myUpdateInfo | info); }
inline void ZLTreeHandler::resetUpdateInfo() { myUpdateInfo = UPDATE_NONE; }

inline const std::string &ZLTreeNode::id() const { return myId; }
inline const std::string &ZLTreeNode::displayName() const { return myDisplayName; }
inline const std::string &ZLTreeNode::pixmapName() const { return myPixmapName; }

inline const ZLTreeHandler &ZLSelectionDialog::handler() const { return myHandler; }

#endif /* __ZLSELECTIONDIALOG_H__ */
