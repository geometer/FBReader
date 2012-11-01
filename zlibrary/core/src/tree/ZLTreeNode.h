/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTREENODE_H__
#define __ZLTREENODE_H__

#include <map>
#include <vector>

#include <shared_ptr.h>

#include <ZLTypeId.h>
#include <ZLRunnable.h>
#include <ZLPaintContext.h>
#include <ZLNetworkRequest.h>

// Nowadays I see hierarchy in this way:
//
// ZLTreeNode
// |
// +-ZLTreeTitledNode -- simple node, as shown below
// | |
// | +- ZLTreeActionNode -- node with some action on click, i.e. BookNode at local library
// |
// +-ZLTreePageNode -- usually the leaf of the tree, smth like page with ZLDialogContent
//                     and some actions, i.e. BookInfo page at network library

class ZLTreeListener;

//class ZLTreeAction : public ZLNetworkRequest { //using ZLRunnableWithKey instead
//public:
//	static const ZLTypeId TYPE_ID;
	
//	ZLTreeAction();
//	~ZLTreeAction();
	
//	const ZLTypeId &typeId() const;

//	virtual void run() = 0;
//	virtual ZLResourceKey key() const = 0;
//	virtual std::string text(const ZLResource &resource) const;
//	virtual bool makesSense() const;
//};

typedef ZLRunnableWithKey ZLTreeAction;

class ZLTreeNode : public ZLObjectWithRTTI {

public:
	typedef std::vector<ZLTreeNode*> List;

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;

protected:
	ZLTreeNode(ZLTreeNode *parent = 0, size_t position = -1);

public:
	virtual ~ZLTreeNode();
	void clear();

public:
	ZLTreeNode *parent() const;
	ZLTreeNode *previous() const;
	ZLTreeNode *next() const;
	const List &children() const;
	size_t childIndex() const;
	// Children should be requested from network only if we need them
	virtual void requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener = 0);
	
	virtual const ZLResource &resource() const;
	void registerAction(shared_ptr<ZLTreeAction> action);
	const std::vector<shared_ptr<ZLTreeAction> > &actions() const;
	std::string actionText(const shared_ptr<ZLTreeAction> &action) const;
	
	void close() const;
	void insert(ZLTreeNode *node, size_t index);
	void append(ZLTreeNode *node);
	void remove(ZLTreeNode *node);
	void remove(size_t index);
	void updated();

	void notifyDownloadStarted();
	void notifyDownloadStopped();

	void notifySearchStarted();
	void notifySearchStopped();

protected:
	size_t level() const;

	virtual ZLTreeListener *listener() const;

private:
	ZLTreeNode *myParent;
	size_t myChildIndex;

	List myChildren;
	// Looks like we should also handle actions for menu on "Tap and hold"
	std::vector<shared_ptr<ZLTreeAction> > myActions;

private:
	ZLTreeNode(const ZLTreeNode&);
	const ZLTreeNode &operator = (const ZLTreeNode&);
};

#endif /* __ZLTREENODE_H__ */
