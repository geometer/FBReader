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

const std::string NetworkAuthorNode::TYPE_ID = "NetworkAuthorNode";

const std::string &NetworkAuthorNode::typeId() const {
	return TYPE_ID;
}

NetworkAuthorNode::NetworkAuthorNode(NetworkContainerNode *parent, const NetworkLibraryBookItem::AuthorData &author) : NetworkContainerNode(parent), myAuthor(author) {
}

void NetworkAuthorNode::paint(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("networkView")["authorNode"];

	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);
	drawTitle(context, vOffset, myAuthor.DisplayName);

	int left = 0;
	drawHyperlink(
		context, left, vOffset,
		resource[isOpen() ? "collapseTree" : "expandTree"].value(),
		expandTreeAction()
	);
}

shared_ptr<ZLImage> NetworkAuthorNode::extractCoverImage() const {
	return defaultCoverImage("booktree-author.png");
}

const NetworkLibraryBookItem::AuthorData &NetworkAuthorNode::author() {
	return myAuthor;
}
