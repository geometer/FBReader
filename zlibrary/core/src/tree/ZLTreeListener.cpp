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

#include "ZLTreeListener.h"

const ZLTypeId ZLTreeListener::RootNode::TYPE_ID(ZLTreeNode::TYPE_ID);

ZLTreeListener::RootNode::RootNode(ZLTreeListener &listener)
    : myListener(listener) {
}

const ZLTypeId &ZLTreeListener::RootNode::typeId() const {
	return TYPE_ID;
}

const ZLTypeId ZLTreeListener::TYPE_ID;

ZLTreeListener::ZLTreeListener(ZLPaintContext &context) {
}

const ZLTypeId &ZLTreeListener::typeId() const {
	return TYPE_ID;
}

ZLTreeListener::RootNode &ZLTreeListener::rootNode() {
}

void ZLTreeListener::clear() {
	myRootNode.clear();
}
