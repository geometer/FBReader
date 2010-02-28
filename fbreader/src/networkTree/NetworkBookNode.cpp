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

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../network/NetworkErrors.h"

#include "../library/Book.h"

#include "../fbreader/FBReader.h"

class NetworkBookNode::DownloadAction : public ZLRunnable {

public:
	DownloadAction(shared_ptr<NetworkItem> book, bool demo);
	void run();

private:
	shared_ptr<NetworkItem> myBook;
	const bool myDemo;
};

class NetworkBookNode::ReadAction : public ZLRunnable {

public:
	ReadAction(shared_ptr<NetworkItem> book);
	void run();

private:
	shared_ptr<NetworkItem> myBook;
};

class NetworkBookNode::ReadDemoAction : public ZLRunnable {

public:
	ReadDemoAction(shared_ptr<NetworkItem> book);
	void run();

private:
	shared_ptr<NetworkItem> myBook;
};

class NetworkBookNode::BuyAction : public ZLRunnable {

public:
	BuyAction(shared_ptr<NetworkItem> book);
	void run();

private:
	shared_ptr<NetworkItem> myBook;
};

class NetworkBookNode::DeleteAction : public ZLRunnable {

public:
	DeleteAction(shared_ptr<NetworkItem> book);
	void run();

private:
	void removeFormat(NetworkBookItem &book, NetworkItem::URLType format);

private:
	shared_ptr<NetworkItem> myBook;
};

static const std::string DEMO_SUFFIX = " (фрагмент)";
static const std::string DEMO_TAG = "Фрагмент";


const ZLTypeId NetworkBookNode::TYPE_ID(FBReaderNode::TYPE_ID);

const ZLTypeId &NetworkBookNode::typeId() const {
	return TYPE_ID;
}

NetworkBookNode::NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book) : FBReaderNode(parent), myBook(book) {
	init();
}

void NetworkBookNode::init() {
	myReadAction = new ReadAction(myBook);
	myDownloadAction = new DownloadAction(myBook, false);
	myReadDemoAction = new ReadDemoAction(myBook);
	myDownloadDemoAction = new DownloadAction(myBook, true);
	myBuyAction = new BuyAction(myBook);
	myDeleteAction = new DeleteAction(myBook);
}

std::string NetworkBookNode::title() const {
	return myBook->Title;
}

std::string NetworkBookNode::summary() const {
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = bookItem().Authors;
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
		NetworkItem::URLType format = book.bestDemoFormat();
		if (format != NetworkItem::URL_NONE) {
			if (hasLocalCopy(book, format)) {
				drawHyperlink(context, left, vOffset, resource["readDemo"].value(), myReadDemoAction);
			} else {
				drawHyperlink(context, left, vOffset, resource["downloadDemo"].value(), myDownloadDemoAction);
			}
		}
		if (purchase) {
			const std::string buyText = ZLStringUtil::printf(resource["buy"].value(), book.Price);
			drawHyperlink(context, left, vOffset, buyText, myBuyAction);
		}
	}
}

shared_ptr<ZLImage> NetworkBookNode::extractCoverImage() const {
	shared_ptr<ZLImage> image = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_COVER]);
	return !image.isNull() ? image : defaultCoverImage("booktree-book.png");
}

bool NetworkBookNode::hasLocalCopy() {
	NetworkBookItem &book = bookItem();
	if (hasLocalCopy(book, NetworkItem::URL_BOOK_EPUB)
		|| hasLocalCopy(book, NetworkItem::URL_BOOK_MOBIPOCKET)
		|| hasLocalCopy(book, NetworkItem::URL_BOOK_FB2_ZIP)) {
		return true;
	}
	if (book.Link.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
	const std::string networkBookId = mgr.networkBookId(book);
	if (!networkBookId.empty()) {
		std::string fileName = NetworkLinkCollection::Instance().bookFileName(networkBookId);
		if (!fileName.empty() && ZLFile(fileName).exists()) {
			return true;
		}
		NetworkItem::URLType format = mgr.downloadLinkType(book);
		fileName = NetworkLinkCollection::Instance().makeBookFileName(networkBookId, format);
		if (!fileName.empty() && ZLFile(fileName).exists()) {
			return true;
		}
	}
	return false;	
}

bool NetworkBookNode::hasLocalCopy(NetworkBookItem &book, NetworkItem::URLType format) {
	std::map<NetworkItem::URLType, std::string>::const_iterator it = book.URLByType.find(format);
	if (it == book.URLByType.end()) {
		return false;
	}
	std::string fileName = NetworkLinkCollection::Instance().bookFileName(it->second);
	if (!fileName.empty() && ZLFile(fileName).exists()) {
		return true;
	}
	fileName = NetworkLinkCollection::Instance().makeBookFileName(it->second, format);
	return !fileName.empty() && ZLFile(fileName).exists();
}

bool NetworkBookNode::NetworkBookNode::hasDirectLink() {
	NetworkBookItem &book = bookItem();
	if (book.bestBookFormat() != NetworkItem::URL_NONE) {
		return true;
	}
	if (book.Link.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
	if (mgr.isAuthorised().Status == B3_TRUE && !mgr.needPurchase(book)) {
		return true;
	}
	return false;
}

bool NetworkBookNode::canBePurchased() {
	NetworkBookItem &book = bookItem();
	if (book.Link.authenticationManager().isNull()) {
		return false;
	}
	NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
	if (mgr.isAuthorised().Status != B3_TRUE) {
		return true;
	}
	return mgr.needPurchase(book);
}

NetworkBookNode::ReadAction::ReadAction(shared_ptr<NetworkItem> book) : myBook(book) {
}

void NetworkBookNode::ReadAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	NetworkItem::URLType format = NetworkItem::URL_NONE;

	if (NetworkBookNode::hasLocalCopy(book, NetworkItem::URL_BOOK_EPUB)) {
		format = NetworkItem::URL_BOOK_EPUB;
	} else if (NetworkBookNode::hasLocalCopy(book, NetworkItem::URL_BOOK_MOBIPOCKET)) {
		format = NetworkItem::URL_BOOK_MOBIPOCKET;
	} else if (NetworkBookNode::hasLocalCopy(book, NetworkItem::URL_BOOK_FB2_ZIP)) {
		format = NetworkItem::URL_BOOK_FB2_ZIP;
	}

	std::string networkBookId;
	if (format != NetworkItem::URL_NONE) {
		networkBookId = book.URLByType[format];
	} else if (!book.Link.authenticationManager().isNull()) {
		NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
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

NetworkBookNode::DownloadAction::DownloadAction(shared_ptr<NetworkItem> book, bool demo) : myBook(book), myDemo(demo) {
}

void NetworkBookNode::DownloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	NetworkItem::URLType format =
		myDemo ? book.bestDemoFormat() : book.bestBookFormat();

	if (format == NetworkItem::URL_NONE) {
		if (myDemo || book.Link.authenticationManager().isNull()) {
			return;
		}
		NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
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
		const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.Title);
		ZLDialogManager::Instance().errorBox(boxKey, message);
		fbreader.refreshWindow();
		return;
	}

	downloaderBook->removeAllAuthors();
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = book.Authors.begin(); it != book.Authors.end(); ++it) {
		downloaderBook->addAuthor(it->DisplayName, it->SortKey);
	}
	downloaderBook->setTitle(myDemo ? book.Title + DEMO_SUFFIX : book.Title);
	downloaderBook->setLanguage(book.Language);
	for (std::vector<std::string>::const_iterator it = book.Tags.begin(); it != book.Tags.end(); ++it) {
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

NetworkBookNode::ReadDemoAction::ReadDemoAction(shared_ptr<NetworkItem> book) : myBook(book) {
};

void NetworkBookNode::ReadDemoAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;
	NetworkItem::URLType format = book.bestDemoFormat();
	if (format == NetworkItem::URL_NONE) {
		return;
	}
	std::string networkBookId = book.URLByType[format];
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

NetworkBookNode::BuyAction::BuyAction(shared_ptr<NetworkItem> book) : myBook(book) {
}

void NetworkBookNode::BuyAction::run() {
	FBReader &fbreader = FBReader::Instance();
	NetworkBookItem &book = (NetworkBookItem &) *myBook;
	if (book.Link.authenticationManager().isNull()) {
		return;
	}
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}
	NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
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
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.Title);
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

NetworkBookNode::DeleteAction::DeleteAction(shared_ptr<NetworkItem> book) : myBook(book) {
}

void NetworkBookNode::DeleteAction::removeFormat(NetworkBookItem &book, NetworkItem::URLType format) {
	std::map<NetworkItem::URLType, std::string>::const_iterator it = book.URLByType.find(format);
	if (it == book.URLByType.end()) {
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

void NetworkBookNode::DeleteAction::run() {
	NetworkBookItem &book = (NetworkBookItem &) *myBook;

	ZLResourceKey boxKey("deleteLocalCopyBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), book.Title);
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}

	removeFormat(book, NetworkItem::URL_BOOK_EPUB);
	removeFormat(book, NetworkItem::URL_BOOK_MOBIPOCKET);
	removeFormat(book, NetworkItem::URL_BOOK_FB2_ZIP);

	if (book.Link.authenticationManager().isNull()) {
		return;
	}

	NetworkAuthenticationManager &mgr = *book.Link.authenticationManager();
	const std::string url = mgr.networkBookId(book);
	const NetworkItem::URLType format = mgr.downloadLinkType(book);
	const std::string fileName = NetworkLinkCollection::Instance().makeBookFileName(url, format);
	if (!fileName.empty()) {
		ZLFile(fileName).remove();
	}
	FBReader::Instance().refreshWindow();
}
