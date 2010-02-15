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
#include <ZLNetworkManager.h>
#include <ZLNetworkUtil.h>
#include <ZLDialogManager.h>

#include "NetworkNodes.h"
#include "NetworkView.h"
#include "NetworkCatalogUtil.h"
#include "NetworkOperationRunnable.h"
#include "AuthenticationDialog.h"

#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkAuthenticationManager.h"
#include "../network/NetworkErrors.h"

#include "../library/Book.h"

#include "../fbreader/FBReader.h"

class NetworkBookInfoNode::DownloadAction : public ZLRunnable {

public:
	DownloadAction(shared_ptr<NetworkLibraryItem> book, bool demo);
	void run();

private:
	shared_ptr<NetworkLibraryItem> myBook;
	const bool myDemo;
};

class NetworkBookInfoNode::ReadAction : public ZLRunnable {

public:
	ReadAction(shared_ptr<NetworkLibraryItem> book);
	void run();

private:
	shared_ptr<NetworkLibraryItem> myBook;
};

class NetworkBookInfoNode::ReadDemoAction : public ZLRunnable {

public:
	ReadDemoAction(shared_ptr<NetworkLibraryItem> book);
	void run();

private:
	shared_ptr<NetworkLibraryItem> myBook;
};

class NetworkBookInfoNode::BuyAction : public ZLRunnable {

public:
	BuyAction(shared_ptr<NetworkLibraryItem> book);
	void run();

private:
	shared_ptr<NetworkLibraryItem> myBook;
};

class NetworkBookInfoNode::DeleteAction : public ZLRunnable {

public:
	DeleteAction(shared_ptr<NetworkLibraryItem> book);
	void run();

private:
	void removeFormat(NetworkBookItem &book, NetworkBookItem::URLType format);

private:
	shared_ptr<NetworkLibraryItem> myBook;
};

static const std::string DEMO_SUFFIX = " (фрагмент)";
static const std::string DEMO_TAG = "Фрагмент";


const ZLTypeId NetworkBookInfoNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &NetworkBookInfoNode::typeId() const {
	return TYPE_ID;
}

NetworkBookInfoNode::NetworkBookInfoNode(NetworkContainerNode *parent, shared_ptr<NetworkLibraryItem> book) : FBReaderNode(parent), myBook(book) {
	init();
}

void NetworkBookInfoNode::init() {
	myReadAction = new ReadAction(myBook);
	myDownloadAction = new DownloadAction(myBook, false);
	myReadDemoAction = new ReadDemoAction(myBook);
	myDownloadDemoAction = new DownloadAction(myBook, true);
	myBuyAction = new BuyAction(myBook);
	myDeleteAction = new DeleteAction(myBook);
}

std::string NetworkBookInfoNode::title() const {
	return bookItem().title();
}

std::string NetworkBookInfoNode::summary() const {
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = bookItem().authors();
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

void NetworkBookInfoNode::paint(ZLPaintContext &context, int vOffset) {
	NetworkBookItem &book = bookItem();

	const ZLResource &resource =
		ZLResource::resource("networkView")["bookNode"];

	removeAllHyperlinks();

	((NetworkView&)view()).drawCoverLater(this, vOffset);

	const bool direct = hasDirectLink();
	const bool purchase = canBePurchased();
	const bool local = hasLocalCopy();

	drawTitle(context, vOffset);
	drawSummary(context, vOffset);

	int left = 0;
	if (local) {
		drawHyperlink(context, left, vOffset, resource["read"].value(), myReadAction);
		drawHyperlink(context, left, vOffset, resource["delete"].value(), myDeleteAction);
	} else if (direct) {
		drawHyperlink(context, left, vOffset, resource["download"].value(), myDownloadAction);
	} else {
		NetworkBookItem::URLType format = book.bestDemoFormat();
		if (format != NetworkBookItem::NONE) {
			if (hasLocalCopy(book, format)) {
				drawHyperlink(context, left, vOffset, resource["readDemo"].value(), myReadDemoAction);
			} else {
				drawHyperlink(context, left, vOffset, resource["downloadDemo"].value(), myDownloadDemoAction);
			}
		}
		if (purchase) {
			const std::string buyText = ZLStringUtil::printf(resource["buy"].value(), book.price());
			drawHyperlink(context, left, vOffset, buyText, myBuyAction);
		}
	}
}

shared_ptr<ZLImage> NetworkBookInfoNode::extractCoverImage() const {
	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->coverURL());
	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
}

bool NetworkBookInfoNode::hasLocalCopy() {
	NetworkBookItem &book = bookItem();
	if (hasLocalCopy(book, NetworkBookItem::BOOK_EPUB)
		|| hasLocalCopy(book, NetworkBookItem::BOOK_MOBIPOCKET)
		|| hasLocalCopy(book, NetworkBookItem::BOOK_FB2_ZIP)) {
		return true;
	}
	if (book.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.authenticationManager();
	const std::string networkBookId = mgr.networkBookId(book);
	if (!networkBookId.empty()) {
		std::string fileName = NetworkLinkCollection::Instance().bookFileName(networkBookId);
		if (!fileName.empty() && ZLFile(fileName).exists()) {
			return true;
		}
		NetworkBookItem::URLType format = mgr.downloadLinkType(book);
		fileName = NetworkLinkCollection::Instance().makeBookFileName(networkBookId, format);
		if (!fileName.empty() && ZLFile(fileName).exists()) {
			return true;
		}
	}
	return false;	
}

bool NetworkBookInfoNode::hasLocalCopy(NetworkBookItem &book, NetworkBookItem::URLType format) {
	std::map<NetworkBookItem::URLType, std::string>::const_iterator it = book.urlByType().find(format);
	if (it == book.urlByType().end()) {
		return false;
	}
	std::string fileName = NetworkLinkCollection::Instance().bookFileName(it->second);
	if (!fileName.empty() && ZLFile(fileName).exists()) {
		return true;
	}
	fileName = NetworkLinkCollection::Instance().makeBookFileName(it->second, format);
	return !fileName.empty() && ZLFile(fileName).exists();
}

bool NetworkBookInfoNode::NetworkBookInfoNode::hasDirectLink() {
	NetworkBookItem &book = bookItem();
	if (book.bestBookFormat() != NetworkBookItem::NONE) {
		return true;
	}
	if (book.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.authenticationManager();
	if (mgr.isAuthorised().Status == B3_TRUE && !mgr.needPurchase(book)) {
		return true;
	}
	return false;
}

bool NetworkBookInfoNode::canBePurchased() {
	NetworkBookItem &book = bookItem();
	if (book.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.authenticationManager();
	if (mgr.isAuthorised().Status != B3_TRUE) {
		return true;
	}
	return mgr.needPurchase(book);
}

NetworkBookInfoNode::ReadAction::ReadAction(shared_ptr<NetworkLibraryItem> book) : myBook(book) {
}

void NetworkBookInfoNode::ReadAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	NetworkBookItem::URLType format = NetworkBookItem::NONE;

	if (NetworkBookInfoNode::hasLocalCopy(book, NetworkBookItem::BOOK_EPUB)) {
		format = NetworkBookItem::BOOK_EPUB;
	} else if (NetworkBookInfoNode::hasLocalCopy(book, NetworkBookItem::BOOK_MOBIPOCKET)) {
		format = NetworkBookItem::BOOK_MOBIPOCKET;
	} else if (NetworkBookInfoNode::hasLocalCopy(book, NetworkBookItem::BOOK_FB2_ZIP)) {
		format = NetworkBookItem::BOOK_FB2_ZIP;
	}

	std::string networkBookId;
	if (format != NetworkBookItem::NONE) {
		networkBookId = book.urlByType()[format];
	} else if (!book.authenticationManager().isNull()) {
		NetworkAuthenticationManager &mgr = *book.authenticationManager();
		networkBookId = mgr.networkBookId(book);
		format = mgr.downloadLinkType(book);
	}
	if (!networkBookId.empty()) {
		std::string fileName = NetworkLinkCollection::Instance().bookFileName(networkBookId);
		if (fileName.empty() || !ZLFile(fileName).exists()) {
			fileName = NetworkLinkCollection::Instance().makeBookFileName(networkBookId, format);
		}
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

NetworkBookInfoNode::DownloadAction::DownloadAction(shared_ptr<NetworkLibraryItem> book, bool demo) : myBook(book), myDemo(demo) {
}

void NetworkBookInfoNode::DownloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	NetworkBookItem::URLType format =
		myDemo ? book.bestDemoFormat() : book.bestBookFormat();

	if (format == NetworkBookItem::NONE) {
		if (myDemo || book.authenticationManager().isNull()) {
			return;
		}
		NetworkAuthenticationManager &mgr = *book.authenticationManager();
		if (mgr.needPurchase(book)) {
			return;
		}
		format = mgr.downloadLinkType(book);
	}

	DownloadBookRunnable downloader(book, format);
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
		const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.title());
		ZLDialogManager::Instance().errorBox(boxKey, message);
		fbreader.refreshWindow();
		return;
	}

	downloaderBook->removeAllAuthors();
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = book.authors().begin(); it != book.authors().end(); ++it) {
		downloaderBook->addAuthor(it->DisplayName, it->SortKey);
	}
	downloaderBook->setTitle(myDemo ? book.title() + DEMO_SUFFIX : book.title());
	downloaderBook->setLanguage(book.language());
	for (std::vector<std::string>::const_iterator it = book.tags().begin(); it != book.tags().end(); ++it) {
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

NetworkBookInfoNode::ReadDemoAction::ReadDemoAction(shared_ptr<NetworkLibraryItem> book) : myBook(book) {
};

void NetworkBookInfoNode::ReadDemoAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;
	NetworkBookItem::URLType format = book.bestDemoFormat();
	if (format == NetworkBookItem::NONE) {
		return;
	}
	std::string networkBookId = book.urlByType()[format];
	if (!networkBookId.empty()) {
		std::string fileName = NetworkLinkCollection::Instance().bookFileName(networkBookId);
		if (fileName.empty() || !ZLFile(fileName).exists()) {
			fileName = NetworkLinkCollection::Instance().makeBookFileName(networkBookId, format);
		}
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

NetworkBookInfoNode::BuyAction::BuyAction(shared_ptr<NetworkLibraryItem> book) : myBook(book) {
}

void NetworkBookInfoNode::BuyAction::run() {
	FBReader &fbreader = FBReader::Instance();
	NetworkBookItem &book = (NetworkBookItem &) *myBook;
	if (book.authenticationManager().isNull()) {
		return;
	}
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}
	NetworkAuthenticationManager &mgr = *book.authenticationManager();
	if (mgr.isAuthorised().Status != B3_TRUE) {
		if (!AuthenticationDialog::run(mgr)) {
			return;
		}
		fbreader.invalidateAccountDependents();
		fbreader.refreshWindow();
		if (!mgr.needPurchase(book)) {
			return;
		}
	}
	ZLResourceKey boxKey("purchaseConfirmBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.title());
	const int code = ZLDialogManager::Instance().questionBox(boxKey, message, ZLResourceKey("buy"), ZLResourceKey("buyAndDownload"), ZLDialogManager::CANCEL_BUTTON);
	if (code == 2) {
		return;
	}
	bool downloadBook = code == 1;
	if (mgr.needPurchase(book)) {
		PurchaseBookRunnable purchaser(mgr, book);
		purchaser.executeWithUI();
		if (purchaser.hasErrors()) {
			purchaser.showErrorMessage();
			downloadBook = false;
		}
	}
	if (downloadBook) {
		DownloadAction(myBook, false).run();
	}
	if (mgr.isAuthorised().Status == B3_FALSE) {
		fbreader.invalidateAccountDependents();
	}
	fbreader.refreshWindow();
}

NetworkBookInfoNode::DeleteAction::DeleteAction(shared_ptr<NetworkLibraryItem> book) : myBook(book) {
}

void NetworkBookInfoNode::DeleteAction::removeFormat(NetworkBookItem &book, NetworkBookItem::URLType format) {
	std::map<NetworkBookItem::URLType, std::string>::const_iterator it = book.urlByType().find(format);
	if (it == book.urlByType().end()) {
		return;
	}
	std::string fileName = NetworkLinkCollection::Instance().bookFileName(it->second);
	if (!fileName.empty()) {
		ZLFile(fileName).remove();
		// TODO: cleanup network database ???
	}
	fileName = NetworkLinkCollection::Instance().makeBookFileName(it->second, format);
	if (!fileName.empty()) {
		ZLFile(fileName).remove();
	}
}

void NetworkBookInfoNode::DeleteAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	ZLResourceKey boxKey("deleteLocalCopyBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.title());
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}

	removeFormat(book, NetworkBookItem::BOOK_EPUB);
	removeFormat(book, NetworkBookItem::BOOK_MOBIPOCKET);
	removeFormat(book, NetworkBookItem::BOOK_FB2_ZIP);

	if (book.authenticationManager().isNull()) {
		return;
	}

	NetworkAuthenticationManager &mgr = *book.authenticationManager();
	const std::string url = mgr.networkBookId(book);
	const NetworkBookItem::URLType format = mgr.downloadLinkType(book);
	const std::string fileName = NetworkLinkCollection::Instance().makeBookFileName(url, format);
	if (!fileName.empty()) {
		ZLFile(fileName).remove();
	}
	FBReader::Instance().refreshWindow();
}

