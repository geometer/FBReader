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

#include <ZLImage.h>
#include <ZLResource.h>

#include "LibraryNodes.h"

#include "../library/Book.h"

const ZLTypeId SeriesNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLResource &SeriesNode::resource() const {
	return ZLResource::resource("libraryView")["seriesNode"];
}

const ZLTypeId &SeriesNode::typeId() const {
	return TYPE_ID;
}

SeriesNode::SeriesNode(AuthorNode *parent) : FBReaderNode(parent) {
}

void SeriesNode::init() {
	registerExpandTreeAction();
}

shared_ptr<Book> SeriesNode::book() const {
	return ((BookNode&)*children().front()).book();
}

std::string SeriesNode::title() const {
	return book()->seriesTitle();
}

shared_ptr<ZLImage> SeriesNode::extractCoverImage() const {
	const std::vector<ZLBlockTreeNode*> &books = children();
	for (std::vector<ZLBlockTreeNode*>::const_iterator it = books.begin(); it != books.end(); ++it) {
		shared_ptr<ZLImage> bookCover = ((FBReaderNode*)*it)->coverImage();
		if (!bookCover.isNull()) {
			return bookCover;
		}
	}
	return 0;
}
