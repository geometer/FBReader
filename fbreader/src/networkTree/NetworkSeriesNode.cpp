/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

const std::string NetworkSeriesNode::TYPE_ID = "NetworkSeriesNode";

const std::string &NetworkSeriesNode::typeId() const {
	return TYPE_ID;
}

NetworkSeriesNode::NetworkSeriesNode(NetworkAuthorNode *parent, const std::string &seriesTitle) : FBReaderNode(parent), mySeriesTitle(seriesTitle) {
}

NetworkSeriesNode::NetworkSeriesNode(NetworkCatalogNode *parent, const std::string &seriesTitle) : FBReaderNode(parent), mySeriesTitle(seriesTitle) {
}

void NetworkSeriesNode::paint(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("networkView")["seriesNode"];

	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);
	drawTitle(context, vOffset, mySeriesTitle);

	int left = 0;
	drawHyperlink(
		context, left, vOffset,
		resource[isOpen() ? "collapseTree" : "expandTree"].value(),
		expandTreeAction()
	);
}

shared_ptr<ZLImage> NetworkSeriesNode::extractCoverImage() const {
	const std::vector<ZLBlockTreeNode*> &books = children();
	for (std::vector<ZLBlockTreeNode*>::const_iterator it = books.begin(); it != books.end(); ++it) {
		shared_ptr<ZLImage> bookCover = ((FBReaderNode*)*it)->coverImage();
		if (!bookCover.isNull()) {
			return bookCover;
		}
	}
	return defaultCoverImage("booktree-book.png");
}

const std::string &NetworkSeriesNode::seriesTitle() {
	return mySeriesTitle;
}
