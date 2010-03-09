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
#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLDialogManager.h>

#include "NetworkNodes.h"
#include "NetworkView.h"
#include "NetworkCatalogUtil.h"
#include "NetworkOperationRunnable.h"
#include "AuthenticationDialog.h"

#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

#include "../library/Book.h"

#include "../fbreader/FBReader.h"

class NetworkBookNode::DownloadAction : public ZLNamedRunnable {

public:
	DownloadAction(const NetworkBookItem &item, bool demo);
	ZLResourceKey key() const;
	void run();

private:
	const NetworkBookItem &myItem;
	const bool myDemo;
};

class NetworkBookNode::ReadAction : public ZLNamedRunnable {

public:
	ReadAction(const NetworkBookItem &item);
	ZLResourceKey key() const;
	void run();

private:
	const NetworkBookItem &myItem;
};

class NetworkBookNode::ReadDemoAction : public ZLNamedRunnable {

public:
	ReadDemoAction(const NetworkBookItem &item);
	ZLResourceKey key() const;
	void run();

private:
	const NetworkBookItem &myItem;
};

class NetworkBookNode::BuyAction : public ZLNamedRunnable {

public:
	BuyAction(const NetworkBookItem &item);
	ZLResourceKey key() const;
	void run();

private:
	const NetworkBookItem &myItem;
};

class NetworkBookNode::DeleteAction : public ZLNamedRunnable {

public:
	DeleteAction(const NetworkBookItem &item);
	ZLResourceKey key() const;
	void run();

private:
	const NetworkBookItem &myItem;
};

static const std::string DEMO_SUFFIX = " (фрагмент)";
static const std::string DEMO_TAG = "Фрагмент";


const ZLTypeId NetworkBookNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &NetworkBookNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkBookNode::resource() const {
	return ZLResource::resource("networkView")["bookNode"];
}

NetworkBookNode::NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book) : FBReaderNode(parent), myBook(book) {
	init();
}

void NetworkBookNode::init() {
	const NetworkBookItem &book = this->book();
	myReadAction = new ReadAction(book);
	myDownloadAction = new DownloadAction(book, false);
	myReadDemoAction = new ReadDemoAction(book);
	myDownloadDemoAction = new DownloadAction(book, true);
	myBuyAction = new BuyAction(book);
	myDeleteAction = new DeleteAction(book);
}

std::string NetworkBookNode::title() const {
	return myBook->Title;
}

std::string NetworkBookNode::summary() const {
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = book().Authors;
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if (!authorsString.empty()) {
			authorsString += ", ";
		}
		authorsString += it->DisplayName;
	}
	FBReaderNode *parent = (FBReaderNode*)this->parent();
	if (parent->title() == authorsString) {
		return std::string();
	}
	return authorsString;
}

void NetworkBookNode::paint(ZLPaintContext &context, int vOffset) {
	const NetworkBookItem &book = this->book();

	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);

	drawTitle(context, vOffset);
	drawSummary(context, vOffset);

	int left = 0;
	if (!book.localCopyFileName().empty()) {
		drawHyperlink(context, left, vOffset, myReadAction);
		drawHyperlink(context, left, vOffset, myDeleteAction);
	} else if (!book.reference(BookReference::DOWNLOAD).isNull()) {
		drawHyperlink(context, left, vOffset, myDownloadAction);
	} else {
		shared_ptr<BookReference> reference =
			book.reference(BookReference::DOWNLOAD_DEMO);
		if (!reference.isNull()) {
			if (!reference->localCopyFileName().empty()) {
				drawHyperlink(context, left, vOffset, myReadDemoAction);
			} else {
				drawHyperlink(context, left, vOffset, myDownloadDemoAction);
			}
		}
		reference = book.reference(BookReference::BUY);
		if (!reference.isNull()) {
			const std::string buyText = ZLStringUtil::printf(
				resource()["buy"].value(), ((BuyBookReference&)*reference).Price
			);
			drawHyperlink(context, left, vOffset, myBuyAction, buyText);
		}
	}
}

shared_ptr<ZLImage> NetworkBookNode::extractCoverImage() const {
	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_COVER]);
	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
}

NetworkBookNode::ReadAction::ReadAction(const NetworkBookItem &item) : myItem(item) {
}

ZLResourceKey NetworkBookNode::ReadAction::key() const {
	return ZLResourceKey("read");
}

void NetworkBookNode::ReadAction::run() {
	const std::string fileName = myItem.localCopyFileName();
	if (!fileName.empty()) {
		FBReader &fbreader = FBReader::Instance();
		shared_ptr<Book> bookPtr;
		fbreader.createBook(fileName, bookPtr);
		if (!bookPtr.isNull()) {
			fbreader.openBook(bookPtr);
			fbreader.setMode(FBReader::BOOK_TEXT_MODE);
			fbreader.refreshWindow();
		}
	}
}

NetworkBookNode::DownloadAction::DownloadAction(const NetworkBookItem &item, bool demo) : myItem(item), myDemo(demo) {
}

ZLResourceKey NetworkBookNode::DownloadAction::key() const {
	return ZLResourceKey(myDemo ? "downloadDemo" : "download");
}

void NetworkBookNode::DownloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	shared_ptr<BookReference> reference = myItem.reference(
		myDemo ? BookReference::DOWNLOAD_DEMO : BookReference::DOWNLOAD
	);
	if (reference.isNull()) {
		return;
	}

	DownloadBookRunnable downloader(reference, myItem.Link.authenticationManager());
	downloader.executeWithUI();
	if (downloader.hasErrors()) {
		downloader.showErrorMessage();
		return;
	}

	FBReader &fbreader = FBReader::Instance();
	shared_ptr<Book> downloaderBook;
	const std::string fileName = downloader.fileName();
	fbreader.createBook(fileName, downloaderBook);
	if (downloaderBook.isNull()) {
		ZLFile(fileName).remove();
		ZLResourceKey boxKey("cantOpenDownloadedFile");
		const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myItem.Title);
		ZLDialogManager::Instance().errorBox(boxKey, message);
		fbreader.refreshWindow();
		return;
	}

	downloaderBook->removeAllAuthors();
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = myItem.Authors.begin(); it != myItem.Authors.end(); ++it) {
		downloaderBook->addAuthor(it->DisplayName, it->SortKey);
	}
	downloaderBook->setTitle(myDemo ? myItem.Title + DEMO_SUFFIX : myItem.Title);
	downloaderBook->setLanguage(myItem.Language);
	for (std::vector<std::string>::const_iterator it = myItem.Tags.begin(); it != myItem.Tags.end(); ++it) {
		downloaderBook->addTag(*it);
	}
	if (myDemo) {
		downloaderBook->addTag(DEMO_TAG);
	}
	Library::Instance().addBook(downloaderBook);

	fbreader.openBook(downloaderBook);
	fbreader.setMode(FBReader::BOOK_TEXT_MODE);
	fbreader.refreshWindow();
}

NetworkBookNode::ReadDemoAction::ReadDemoAction(const NetworkBookItem &item) : myItem(item) {
};

ZLResourceKey NetworkBookNode::ReadDemoAction::key() const {
	return ZLResourceKey("readDemo");
}

void NetworkBookNode::ReadDemoAction::run() {
	shared_ptr<BookReference> reference =
		myItem.reference(BookReference::DOWNLOAD_DEMO);
	const std::string fileName = reference->localCopyFileName();
	if (!fileName.empty()) {
		FBReader &fbreader = FBReader::Instance();
		shared_ptr<Book> bookPtr;
		fbreader.createBook(fileName, bookPtr);
		if (!bookPtr.isNull()) {
			fbreader.openBook(bookPtr);
			fbreader.setMode(FBReader::BOOK_TEXT_MODE);
			fbreader.refreshWindow();
		}
	}
}

NetworkBookNode::BuyAction::BuyAction(const NetworkBookItem &item) : myItem(item) {
}

ZLResourceKey NetworkBookNode::BuyAction::key() const {
	return ZLResourceKey("buy");
}

void NetworkBookNode::BuyAction::run() {
	FBReader &fbreader = FBReader::Instance();
	if (myItem.Link.authenticationManager().isNull()) {
		return;
	}
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}
	NetworkAuthenticationManager &mgr = *myItem.Link.authenticationManager();
	if (mgr.isAuthorised().Status != B3_TRUE) {
		if (!AuthenticationDialog::run(mgr)) {
			return;
		}
		fbreader.invalidateAccountDependents();
		fbreader.refreshWindow();
		if (!mgr.needPurchase(myItem)) {
			return;
		}
	}
	ZLResourceKey boxKey("purchaseConfirmBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myItem.Title);
	const int code = ZLDialogManager::Instance().questionBox(boxKey, message, ZLResourceKey("buy"), ZLResourceKey("buyAndDownload"), ZLDialogManager::CANCEL_BUTTON);
	if (code == 2) {
		return;
	}
	bool downloadBook = code == 1;
	if (mgr.needPurchase(myItem)) {
		PurchaseBookRunnable purchaser(mgr, myItem);
		purchaser.executeWithUI();
		if (purchaser.hasErrors()) {
			purchaser.showErrorMessage();
			downloadBook = false;
		}
	}
	if (downloadBook) {
		DownloadAction(myItem, false).run();
	}
	if (mgr.isAuthorised().Status == B3_FALSE) {
		fbreader.invalidateAccountDependents();
	}
	fbreader.refreshWindow();
}

NetworkBookNode::DeleteAction::DeleteAction(const NetworkBookItem &item) : myItem(item) {
}

ZLResourceKey NetworkBookNode::DeleteAction::key() const {
	return ZLResourceKey("delete");
}

void NetworkBookNode::DeleteAction::run() {
	ZLResourceKey boxKey("deleteLocalCopyBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myItem.Title);
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}

	myItem.removeLocalFiles();
	FBReader::Instance().refreshWindow();
}

const NetworkBookItem &NetworkBookNode::book() const {
	return (const NetworkBookItem&)*myBook;
}
