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

#include "../library/Library.h"

const ZLTypeId TitleNode::TYPE_ID(FBNode::TYPE_ID);

const ZLTypeId &TitleNode::typeId() const {
        return TYPE_ID;
}

TitleNode::TitleNode(std::string firstLetter): myFirstLetter(firstLetter) {
        const BookList &books = Library::Instance().books(myFirstLetter);
        for (BookList::const_iterator it = books.begin(); it != books.end(); ++it) {
                append(new BookNode(*it, BookNode::SHOW_AUTHORS));
        }
}

const ZLResource &TitleNode::resource() const {
        return ZLResource::resource("libraryView")["TitleNode"];
}

std::string TitleNode::title() const {
        return myFirstLetter;
}

std::string TitleNode::subtitle() const {
   return ZLStringUtil::join(Library::Instance().books(myFirstLetter), BookFunctor(), COMMA_JOIN_SEPARATOR);
}

std::string TitleNode::imageUrl() const {
        return FBNode::defaultImageUrl("booktree-books.png");
}
