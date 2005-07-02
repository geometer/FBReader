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

#ifndef __ZLOPENFILEDIALOG_H__
#define __ZLOPENFILEDIALOG_H__

#include <vector>

#include <abstract/shared_ptr.h>
#include <abstract/ZLOptions.h>

class ZLDir;
class ZLTreeState;
typedef shared_ptr<ZLTreeState> ZLTreeStatePtr;
class ZLTreeNode;
typedef shared_ptr<ZLTreeNode> ZLTreeNodePtr;

class ZLTreeHandler {

protected:
	ZLTreeHandler() DIALOG_SECTION;
	virtual ~ZLTreeHandler() DIALOG_SECTION;
	
public:
	virtual bool isNodeVisible(const ZLTreeNodePtr node) const DIALOG_SECTION = 0;
	virtual const std::string &pixmapName(const ZLTreeNodePtr node) const DIALOG_SECTION = 0;
	virtual void accept(const ZLTreeStatePtr state) const DIALOG_SECTION = 0;
};

class ZLTreeNode {

public:
	ZLTreeNode(const std::string &name, bool isFile);
	~ZLTreeNode();

	const std::string &name() const;
	bool isFile() const;
	std::string relativeName(const ZLTreeStatePtr state) const;

private:
	std::string myName;
	bool myIsFile;
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

	const ZLTreeHandler &handler() const DIALOG_SECTION;

private:
	const ZLTreeHandler &myHandler;
};

class ZLDirTreeState : public ZLTreeState {

public:
	ZLDirTreeState(const ZLTreeHandler &handler, shared_ptr<ZLDir> dir);
	~ZLDirTreeState();

	ZLTreeStatePtr change(const ZLTreeNodePtr node);

	const std::vector<ZLTreeNodePtr> &subnodes() const;
	const std::string &name() const;
	const std::string shortName() const;
	bool isLeaf() const;

private:
	void fill() const;

private:
	shared_ptr<ZLDir> myDir;
	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;
};

class ZLFileTreeState : public ZLTreeState {

public:
	ZLFileTreeState(const ZLTreeHandler &handler, const std::string &name);
	~ZLFileTreeState();

	ZLTreeStatePtr change(const ZLTreeNodePtr node);

	const std::vector<ZLTreeNodePtr> &subnodes() const;
	const std::string &name() const;
	const std::string shortName() const;
	bool isLeaf() const;

private:
	void fill() const;

private:
	std::string myFileName;
};

class ZLOpenFileDialog {

public:
	static ZLStringOption DirectoryOption;

protected:
	ZLOpenFileDialog(const ZLTreeHandler &handler) DIALOG_SECTION;
	virtual ~ZLOpenFileDialog() DIALOG_SECTION;

public:
	virtual void run() DIALOG_SECTION = 0;

protected:
	void runNode(const ZLTreeNodePtr node) DIALOG_SECTION;
	virtual void exitDialog() DIALOG_SECTION = 0;
	virtual void update(const std::string &selectedNodeName) DIALOG_SECTION = 0;
	const std::string &pixmapName(const ZLTreeNodePtr node) const DIALOG_SECTION;

protected:
	shared_ptr<ZLTreeState> myCurrentDir;
};

inline ZLTreeHandler::ZLTreeHandler() {}
inline ZLTreeHandler::~ZLTreeHandler() {}
inline const ZLTreeHandler &ZLTreeState::handler() const { return myHandler; }

#endif /* __ZLOPENFILEDIALOG_H__ */
