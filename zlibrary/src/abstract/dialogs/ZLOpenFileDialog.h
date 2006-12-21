/*
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

#ifndef __ZLOPENFILEDIALOG_H__
#define __ZLOPENFILEDIALOG_H__

#include <vector>

#include <shared_ptr.h>
#include <ZLOptions.h>

class ZLDir;
class ZLTreeState;
typedef shared_ptr<ZLTreeState> ZLTreeStatePtr;
class ZLTreeNode;
typedef shared_ptr<ZLTreeNode> ZLTreeNodePtr;

class ZLTreeHandler {

protected:
	ZLTreeHandler();
	virtual ~ZLTreeHandler();
	
public:
	virtual const std::string &pixmapName(const ZLDir &dir, const std::string &name, bool isFile) const = 0;
	virtual bool isAcceptable(const std::string &name) const = 0;
	virtual const std::string accept(const std::string &name) const = 0;
};

class ZLTreeNode {

public:
	ZLTreeNode(const std::string &id, const std::string &displayName, const std::string &pixmapName, bool isFolder);

	const std::string &id() const;
	const std::string &displayName() const;
	const std::string &pixmapName() const;
	bool isFolder() const;
	std::string relativeName(const ZLTreeStatePtr state) const;

private:
	std::string myId;
	std::string myDisplayName;
	std::string myPixmapName;
	bool myIsFolder;
};

class ZLTreeState {

protected:
	ZLTreeState(const ZLTreeHandler &handler);

public:
	virtual ~ZLTreeState();

	virtual ZLTreeStatePtr change(const ZLTreeNodePtr node) = 0;

	virtual const std::vector<ZLTreeNodePtr> &subnodes() const = 0;
	virtual const std::string &name() const = 0;
	virtual const std::string shortName() const = 0;
	virtual bool isLeaf() const = 0;
	virtual bool exists() const = 0;

	const ZLTreeHandler &handler() const;

private:
	const ZLTreeHandler &myHandler;
};

class ZLDirTreeState : public ZLTreeState {

public:
	ZLDirTreeState(const ZLTreeHandler &handler, shared_ptr<ZLDir> dir);

	ZLTreeStatePtr change(const ZLTreeNodePtr node);

	const std::vector<ZLTreeNodePtr> &subnodes() const;
	const std::string &name() const;
	const std::string shortName() const;
	bool isLeaf() const;
	bool exists() const;

private:
	void fill() const;
	void addSubnode(const std::string &name, bool isFile) const;

private:
	shared_ptr<ZLDir> myDir;
	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;
};

class ZLFileTreeState : public ZLTreeState {

public:
	ZLFileTreeState(const ZLTreeHandler &handler, const std::string &name);

	ZLTreeStatePtr change(const ZLTreeNodePtr node);

	const std::vector<ZLTreeNodePtr> &subnodes() const;
	const std::string &name() const;
	const std::string shortName() const;
	bool isLeaf() const;
	bool exists() const;

private:
	void fill() const;

private:
	std::string myFileName;
};

class ZLOpenFileDialog {

public:
	ZLStringOption DirectoryOption;

protected:
	ZLOpenFileDialog(const ZLTreeHandler &handler);
	virtual ~ZLOpenFileDialog();

public:
	virtual void run() = 0;

protected:
	void runNode(const ZLTreeNodePtr node);
	virtual void exitDialog() = 0;
	virtual void update(const std::string &selectedNodeName) = 0;
	const ZLTreeStatePtr state() const;

private:
	ZLTreeStatePtr myState;
};

inline const std::string &ZLTreeNode::id() const { return myId; }
inline const std::string &ZLTreeNode::displayName() const { return myDisplayName; }
inline const std::string &ZLTreeNode::pixmapName() const { return myPixmapName; }

inline ZLTreeHandler::ZLTreeHandler() {}
inline ZLTreeHandler::~ZLTreeHandler() {}
inline const ZLTreeHandler &ZLTreeState::handler() const { return myHandler; }

inline const ZLTreeStatePtr ZLOpenFileDialog::state() const { return myState; }

#endif /* __ZLOPENFILEDIALOG_H__ */
