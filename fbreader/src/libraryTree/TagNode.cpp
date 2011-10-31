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
#include <ZLStringUtil.h>

#include "LibraryNodes.h"

#include "../library/Tag.h"
#include "../library/Library.h"
#include "../libraryActions/LibraryTagActions.h"

const ZLTypeId TagNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &TagNode::typeId() const {
	return TYPE_ID;
}

TagNode::TagNode(shared_ptr<Tag> tag): myTag(tag) {
	//TODO add support for subtags here
	const BookList &books = Library::Instance().books(myTag);
	//TODO add code for series retrieving here
	size_t index = 0;
	for (BookList::const_iterator it = books.begin(); it != books.end(); ++it) {
                insert(new BookNode(*it, BookNode::SHOW_AUTHORS),index++);
	}
}

const ZLResource &TagNode::resource() const {
	return ZLResource::resource("libraryView")["tagNode"];
}

std::string TagNode::title() const {
	if (myTag.isNull()) {
		return resource()["noTags"].value();
	}
	return myTag->name();
}

std::string TagNode::subtitle() const {
   return ZLStringUtil::join(Library::Instance().books(myTag), BookFunctor(), COMMA_JOIN_SEPARATOR);
}

shared_ptr<ZLImage> TagNode::extractCoverImage() const {
	return defaultCoverImage("booktree-tag.png");
}

std::string TagNode::imageUrl() const {
	return defaultImageUrl("booktree-tag.png");
}
