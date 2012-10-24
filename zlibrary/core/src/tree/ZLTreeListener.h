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

#ifndef __ZLTREELISTENER_H__
#define __ZLTREELISTENER_H__

#include "ZLTreeNode.h"

//ZLTreeListener is used to get ability to node to notificate Tree Dialog about any changes

class ZLTreeListener {

public:
	virtual void onExpandRequest(ZLTreeNode *node) = 0;
	virtual void onCloseRequest() = 0;
	// Parent has new or remove child at index
	virtual void onNodeBeginInsert(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndInsert() = 0;
	virtual void onNodeBeginRemove(ZLTreeNode *parent, size_t index) = 0;
	virtual void onNodeEndRemove() = 0;
	// This method should be called at every node state change except of adding/removing of children
	virtual void onNodeUpdated(ZLTreeNode *node) = 0;

	virtual void onDownloadingStarted(ZLTreeNode *node) = 0;
	virtual void onDownloadingStopped(ZLTreeNode *node) = 0;
};

#endif /* __ZLTREELISTENER_H__ */
