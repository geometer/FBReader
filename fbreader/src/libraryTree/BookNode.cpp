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

const ZLTypeId BookNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &BookNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &BookNode::resource() const {
	return ZLResource::resource("libraryView")["bookNode"];
}

BookNode::BookNode(AuthorNode *parent, shared_ptr<Book> book) : FBReaderNode(parent), myBook(book) {
}

BookNode::BookNode(SeriesNode *parent, shared_ptr<Book> book) : FBReaderNode(parent), myBook(book) {
}

BookNode::BookNode(TagNode *parent, size_t atPosition, shared_ptr<Book> book) : FBReaderNode(parent, atPosition), myBook(book) {
}

void BookNode::init() {
	registerAction(new BookReadAction(myBook));
	registerAction(new BookEditInfoAction(myBook));
	registerAction(new BookRemoveAction(myBook));
}

shared_ptr<Book> BookNode::book() const {
	return myBook;
}

std::string BookNode::title() const {
	return myBook->title();
}

std::string BookNode::summary() const {
	FBReaderNode *parent = (FBReaderNode*)this->parent();
	while (!parent->isInstanceOf(AuthorNode::TYPE_ID) &&
				 !parent->isInstanceOf(TagNode::TYPE_ID)) {
		parent = (FBReaderNode*)parent->parent();
	}
	if (parent->isInstanceOf(AuthorNode::TYPE_ID)) {
		const TagList &tags = myBook->tags();
		if (tags.empty()) {
			return std::string();
		} else {
			std::string tagsText;
			for (TagList::const_iterator it = tags.begin(); it != tags.end(); ++it) {
				if (!tagsText.empty()) {
					tagsText += ", ";
				}
				tagsText += (*it)->name();
			}
			return tagsText;
		}
	} else {
		const AuthorList &authors = myBook->authors();
		if (authors.empty()) {
			return ZLResource::resource("libraryView")["authorNode"]["unknownAuthor"].value();
		} else {
			std::string authorsText;
			for (AuthorList::const_iterator it = authors.begin(); it != authors.end(); ++it) {
				if (!authorsText.empty()) {
					authorsText += ", ";
				}
				authorsText += (*it)->name();
			}
			return authorsText;
		}
	}
}

bool BookNode::highlighted() const {
	return myBook->filePath() == FBReader::Instance().currentBook()->filePath();
}

shared_ptr<ZLImage> BookNode::extractCoverImage() const {
	shared_ptr<FormatPlugin> plugin = PluginCollection::Instance().plugin(*myBook);
	if (!plugin.isNull()) {
		shared_ptr<ZLImage> cover = plugin->coverImage(*myBook);
		if (!cover.isNull()) {
			return cover;
		}
	}
	return defaultCoverImage("booktree-book.png");
}
