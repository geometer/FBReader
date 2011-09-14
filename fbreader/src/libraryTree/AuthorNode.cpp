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

#include "../library/Library.h"
#include "../library/Author.h"
#include "../libraryActions/LibraryAuthorActions.h"

const ZLTypeId AuthorNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &AuthorNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &AuthorNode::resource() const {
	return ZLResource::resource("libraryView")["authorNode"];
}

AuthorNode::AuthorNode(shared_ptr<Author> author): myAuthor(author) {
	const BookList &books = Library::Instance().books(myAuthor);
	//TODO add code for series retrieving here
	size_t index = 0;
	for (BookList::const_iterator it = books.begin(); it != books.end(); ++it) {
		insert(new BookNode(*it),index++);
	}
}

std::string AuthorNode::title() const {
	return myAuthor.isNull() ? resource()["unknownAuthor"].value() : myAuthor->name();
}
std::string AuthorNode::subtitle() const {
	return std::string();
}

std::string AuthorNode::imageUrl() const {
	return std::string();
}

shared_ptr<ZLImage> AuthorNode::extractCoverImage() const {
	return defaultCoverImage("booktree-author.png");
}

void AuthorNode::requestChildren() {

}
