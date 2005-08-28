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
	virtual const std::string &pixmapName(const ZLDir &dir, const std::string &name, bool isFile) const DIALOG_SECTION = 0;
	virtual void accept(const ZLTreeState &state) const DIALOG_SECTION = 0;
};

class ZLTreeNode {

public:
	ZLTreeNode(const std::string &name, bool isFile, const std::string &pixmapName) DIALOG_SECTION;
	~ZLTreeNode() DIALOG_SECTION;

	const std::string &name() const DIALOG_SECTION;
	const std::string &pixmapName() const DIALOG_SECTION;
	bool isFile() const DIALOG_SECTION;
	std::string relativeName(const ZLTreeStatePtr state) const DIALOG_SECTION;

private:
	std::string myName;
	bool myIsFile;
	std::string myPixmapName;
};

class ZLTreeState {

protected:
	ZLTreeState(const ZLTreeHandler &handler) DIALOG_SECTION;

public:
	virtual ~ZLTreeState() DIALOG_SECTION;

	virtual ZLTreeStatePtr change(const ZLTreeNodePtr node) DIALOG_SECTION = 0;

	virtual const std::vector<ZLTreeNodePtr> &subnodes() const DIALOG_SECTION = 0;
	virtual const std::string &name() const DIALOG_SECTION = 0;
	virtual const std::string shortName() const DIALOG_SECTION = 0;
	virtual bool isLeaf() const DIALOG_SECTION = 0;

	const ZLTreeHandler &handler() const DIALOG_SECTION;

private:
	const ZLTreeHandler &myHandler;
};

class ZLDirTreeState : public ZLTreeState {

public:
	ZLDirTreeState(const ZLTreeHandler &handler, shared_ptr<ZLDir> dir) DIALOG_SECTION;
	~ZLDirTreeState() DIALOG_SECTION;

	ZLTreeStatePtr change(const ZLTreeNodePtr node) DIALOG_SECTION;

	const std::vector<ZLTreeNodePtr> &subnodes() const DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	const std::string shortName() const DIALOG_SECTION;
	bool isLeaf() const DIALOG_SECTION;

private:
	void fill() const DIALOG_SECTION;

private:
	shared_ptr<ZLDir> myDir;
	mutable bool myIsUpToDate;
	mutable std::vector<ZLTreeNodePtr> mySubnodes;
};

class ZLFileTreeState : public ZLTreeState {

public:
	ZLFileTreeState(const ZLTreeHandler &handler, const std::string &name) DIALOG_SECTION;
	~ZLFileTreeState() DIALOG_SECTION;

	ZLTreeStatePtr change(const ZLTreeNodePtr node) DIALOG_SECTION;

	const std::vector<ZLTreeNodePtr> &subnodes() const DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	const std::string shortName() const DIALOG_SECTION;
	bool isLeaf() const DIALOG_SECTION;

private:
	void fill() const DIALOG_SECTION;

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
	const ZLTreeStatePtr state() const;

private:
	ZLTreeStatePtr myState;
};

inline ZLTreeHandler::ZLTreeHandler() {}
inline ZLTreeHandler::~ZLTreeHandler() {}
inline const ZLTreeHandler &ZLTreeState::handler() const { return myHandler; }

inline const ZLTreeStatePtr ZLOpenFileDialog::state() const { return myState; }

#endif /* __ZLOPENFILEDIALOG_H__ */
