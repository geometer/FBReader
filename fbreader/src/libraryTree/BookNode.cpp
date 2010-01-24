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
#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLImageManager.h>

#include "LibraryNodes.h"

#include "../library/Book.h"
#include "../library/Author.h"
#include "../library/Tag.h"
#include "../fbreader/FBReader.h"
#include "../fbreader/BookInfoDialog.h"
#include "../formats/FormatPlugin.h"

class BookNode::ReadAction : public ZLRunnable {

public:
	ReadAction(shared_ptr<Book> book);
	void run();

private:
	shared_ptr<Book> myBook;
};

class BookNode::EditInfoAction : public ZLRunnable {

public:
	EditInfoAction(shared_ptr<Book> book);
	void run();

private:
	shared_ptr<Book> myBook;
};

class BookNode::RemoveAction : public ZLRunnable {

public:
	RemoveAction(shared_ptr<Book> book);
	void run();

private:
	int removeBookDialog() const;

private:
	shared_ptr<Book> myBook;
};

const std::string BookNode::TYPE_ID = "BookNode";

const std::string &BookNode::typeId() const {
	return TYPE_ID;
}

BookNode::BookNode(AuthorNode *parent, shared_ptr<Book> book) : FBReaderNode(parent), myBook(book) {
}

BookNode::BookNode(SeriesNode *parent, shared_ptr<Book> book) : FBReaderNode(parent), myBook(book) {
}

BookNode::BookNode(TagNode *parent, size_t atPosition, shared_ptr<Book> book) : FBReaderNode(parent, atPosition), myBook(book) {
}

shared_ptr<Book> BookNode::book() const {
	return myBook;
}

BookNode::ReadAction::ReadAction(shared_ptr<Book> book) : myBook(book) {
}

void BookNode::ReadAction::run() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.openBook(myBook);
	fbreader.showBookTextView();
}

BookNode::RemoveAction::RemoveAction(shared_ptr<Book> book) : myBook(book) {
}

void BookNode::RemoveAction::run() {
	switch (removeBookDialog()) {
		case Library::REMOVE_FROM_DISK:
		{
			const std::string path = ZLFile(myBook->filePath()).physicalFilePath();
			ZLFile physicalFile(path);
			if (!physicalFile.remove()) {
				ZLResourceKey boxKey("removeFileErrorBox");
				const std::string message =
					ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), path);
				ZLDialogManager::Instance().errorBox(boxKey, message);
			}
		}
		// yes, we go through this label
		case Library::REMOVE_FROM_LIBRARY:
			Library::Instance().removeBook(myBook);
			FBReader::Instance().refreshWindow();
		case Library::REMOVE_DONT_REMOVE:
			break;
	}
}

int BookNode::RemoveAction::removeBookDialog() const {
	ZLResourceKey boxKey("removeBookBox");
	const ZLResource &msgResource = ZLResource::resource("dialog")[boxKey];

	switch (Library::Instance().canRemove(myBook)) {
		case Library::REMOVE_DONT_REMOVE:
			return Library::REMOVE_DONT_REMOVE;
		case Library::REMOVE_FROM_DISK:
		{
			ZLFile physFile(ZLFile(myBook->filePath()).physicalFilePath());
			const std::string message = ZLStringUtil::printf(msgResource["deleteFile"].value(), physFile.name(false));
			if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) == 0) {
				return Library::REMOVE_FROM_DISK;
			}
			return Library::REMOVE_DONT_REMOVE;
		}
		case Library::REMOVE_FROM_LIBRARY:
		{
			const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook->title());
			if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) == 0) {
				return Library::REMOVE_FROM_LIBRARY;
			}
			return Library::REMOVE_DONT_REMOVE;
		}
		case Library::REMOVE_FROM_LIBRARY_AND_DISK:
		{
			ZLResourceKey removeFileKey("removeFile");
			ZLResourceKey removeLinkKey("removeLink");
    
			const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook->title());
			switch(ZLDialogManager::Instance().questionBox(boxKey, message, removeLinkKey, removeFileKey, ZLDialogManager::CANCEL_BUTTON)) {
				case 0:
					return Library::REMOVE_FROM_LIBRARY;
				case 1:
					return Library::REMOVE_FROM_DISK;
				case 2:
					return Library::REMOVE_DONT_REMOVE;
			}
		}
	}
	return Library::REMOVE_DONT_REMOVE;
}

BookNode::EditInfoAction::EditInfoAction(shared_ptr<Book> book) : myBook(book) {
}

void BookNode::EditInfoAction::run() {
	if (BookInfoDialog(myBook).dialog().run()) {
		// TODO: select current node (?) again
		FBReader::Instance().refreshWindow();
	}
}

std::string BookNode::title() const {
	return myBook->title();
}

std::string BookNode::summary() const {
	FBReaderNode *parent = (FBReaderNode*)this->parent();
	while (parent->typeId() != AuthorNode::TYPE_ID &&
				 parent->typeId() != TagNode::TYPE_ID) {
		parent = (FBReaderNode*)parent->parent();
	}
	if (parent->typeId() == AuthorNode::TYPE_ID) {
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

void BookNode::paint(ZLPaintContext &context, int vOffset) {
	const ZLResource &resource =
		ZLResource::resource("libraryView")["bookNode"];

	const bool highlighted =
		myBook->filePath() == FBReader::Instance().currentBook()->filePath();
	drawCover(context, vOffset);
	drawTitle(context, vOffset, highlighted);
	drawSummary(context, vOffset, highlighted);

	if (myReadAction.isNull()) {
		myReadAction = new ReadAction(myBook);
		myEditInfoAction = new EditInfoAction(myBook);
		Library::RemoveType type = Library::Instance().canRemove(myBook);
		if (type != Library::REMOVE_DONT_REMOVE) {
			myRemoveAction = new RemoveAction(myBook);
		}
	}
	int left = 0;
	drawHyperlink(
		context, left, vOffset,
		resource["read"].value(),
		myReadAction
	);
	drawHyperlink(
		context, left, vOffset,
		resource["edit"].value(),
		myEditInfoAction
	);
	drawHyperlink(
		context, left, vOffset,
		resource["delete"].value(),
		myRemoveAction
	);
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
