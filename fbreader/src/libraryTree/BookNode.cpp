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

#include "LibraryNodes.h"

#include "../library/Book.h"
#include "../library/Author.h"
#include "../library/Tag.h"
#include "../libraryActions/LibraryBookActions.h"

#include "../fbreader/FBReader.h"
#include "../formats/FormatPlugin.h"

const ZLTypeId BookNode::TYPE_ID(ZLTreeActionNode::TYPE_ID);

const ZLTypeId &BookNode::typeId() const {
	return TYPE_ID;
}

BookNode::BookNode(shared_ptr<Book> book): myBook(book) {
}

std::string BookNode::title() const {
	return myBook->title();
}
std::string BookNode::subtitle() const {
	//TODO add more verbose subtitle here
	return myBook->title();
}

std::string BookNode::imageUrl() const {
	return std::string();
}

shared_ptr<ZLImage> BookNode::extractCoverImage() const {
	shared_ptr<FormatPlugin> plugin = PluginCollection::Instance().plugin(*myBook);
	if (!plugin.isNull()) {
		shared_ptr<ZLImage> cover = plugin->coverImage(myBook->file());
		if (!cover.isNull()) {
			return cover;
		}
	}
	return 0; //defaultCoverImage("booktree-book.png");
}

shared_ptr<ZLImage> BookNode::image() const {
	return 0;
}

void BookNode::requestChildren() {
}

bool BookNode::activate() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.openBook(myBook);
	return true;
}

const ZLResource &BookNode::resource() const {
	return ZLResource::resource("libraryView")["bookNode"];
}
