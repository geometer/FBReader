/*
 * Copyright (C) 2009-2011 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTREELISTENER_H__
#define __ZLTREELISTENER_H__

#include "ZLTreeNode.h"

// Listener's methods should be called on changes of the tree,
// i.e. adding of new children or receiving of the node's image.
class ZLTreeListener {
	
public:
	class RootNode : public ZLTreeNode {

	public:
		static const ZLTypeId TYPE_ID;
 
	public:
		RootNode(ZLTreeListener &listener);

	private:
		const ZLTypeId &typeId() const;
		
		ZLTreeListener &myListener;
		
		friend class ZLTreeNode;
	};
	
	static const ZLTypeId TYPE_ID;

protected:
	ZLTreeListener();
	virtual ~ZLTreeListener();
	
	const ZLTypeId &typeId() const;

public: //TODO it's temporary; should it be public or protected?
	RootNode &rootNode();

protected:
	void clear();

public:

	// Parent has new child at index
	virtual void onNodeBeginInsert(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndInsert() = 0;
	// This is also easy to understand
	virtual void onNodeBeginRemove(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndRemove() = 0;
	// This method should be called at every node state change except of adding/removing of children
	virtual void onNodeUpdated(ZLTreeNode *node) = 0;

private:
	RootNode myRootNode;
};

#endif /* __ZLTREELISTENER_H__ */
