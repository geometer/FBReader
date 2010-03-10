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
#include <ZLOptionsDialog.h>

#include "LibraryNodes.h"

#include "../library/Author.h"
#include "../libraryActions/LibraryAuthorActions.h"

const ZLTypeId AuthorNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLResource &AuthorNode::resource() const {
	return ZLResource::resource("libraryView")["authorNode"];
}

const ZLTypeId &AuthorNode::typeId() const {
	return TYPE_ID;
}

AuthorNode::AuthorNode(ZLBlockTreeView::RootNode *parent, size_t atPosition, shared_ptr<Author> author) : FBReaderNode(parent, atPosition), myAuthor(author) {
}

void AuthorNode::init() {
	registerExpandTreeAction();
	if (!myAuthor.isNull()) {
		registerAction(new AuthorEditInfoAction(myAuthor));
	}
}

shared_ptr<Author> AuthorNode::author() const {
	return myAuthor;
}

std::string AuthorNode::title() const {
	return myAuthor.isNull() ?
		resource()["unknownAuthor"].value() : myAuthor->name();
}

shared_ptr<ZLImage> AuthorNode::extractCoverImage() const {
	return defaultCoverImage("booktree-author.png");
}
