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

#include "ZLTreePageNode.h"
#include <ZLDialogManager.h>

const ZLTypeId ZLTreePageNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

ZLTreePageNode::ZLTreePageNode(ZLTreeNode *parent, size_t position) : ZLTreeTitledNode(parent, position) {
}

const ZLTypeId &ZLTreePageNode::typeId() const {
	return TYPE_ID;
}

//shared_ptr<ZLDialogContent> ZLTreePageNode::content() const {
//        // if was commented, because there's necessary to make new content each time,
//        //because object ZLQtDialogContent has inner widget, that can become invalidated
//        //if (myContent.isNull()) {
////                ZLDialogManager &manager = ZLDialogManager::Instance();
////                myContent = manager.createContent(contentKey());
////                fillContent(*myContent);
//        //}
//	return myContent;
//}
