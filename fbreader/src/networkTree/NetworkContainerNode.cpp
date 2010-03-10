/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkNodes.h"
#include "NetworkView.h"

const ZLTypeId NetworkContainerNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &NetworkContainerNode::typeId() const {
	return TYPE_ID;
}

NetworkContainerNode::NetworkContainerNode(ZLBlockTreeView::RootNode *parent, size_t atPosition) : FBReaderNode(parent, atPosition) {
}

NetworkContainerNode::NetworkContainerNode(NetworkContainerNode *parent, size_t atPosition) : FBReaderNode(parent, atPosition) {
}

void NetworkContainerNode::drawCover(ZLPaintContext&, int vOffset) {
	((NetworkView&)view()).drawCoverLater(this, vOffset);
}
