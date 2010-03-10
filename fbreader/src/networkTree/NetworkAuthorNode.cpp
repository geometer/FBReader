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

#include <ZLResource.h>
#include <ZLImage.h>

#include "NetworkNodes.h"
#include "NetworkView.h"

const ZLTypeId NetworkAuthorNode::TYPE_ID(NetworkContainerNode::TYPE_ID);

const ZLTypeId &NetworkAuthorNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkAuthorNode::resource() const {
	return ZLResource::resource("networkView")["authorNode"];
}

NetworkAuthorNode::NetworkAuthorNode(NetworkContainerNode *parent, const NetworkBookItem::AuthorData &author) : NetworkContainerNode(parent), myAuthor(author) {
}

void NetworkAuthorNode::init() {
	registerExpandTreeAction();
}

std::string NetworkAuthorNode::title() const {
	return myAuthor.DisplayName;
}

shared_ptr<ZLImage> NetworkAuthorNode::extractCoverImage() const {
	return defaultCoverImage("booktree-author.png");
}

const NetworkBookItem::AuthorData &NetworkAuthorNode::author() {
	return myAuthor;
}
