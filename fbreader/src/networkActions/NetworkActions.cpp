/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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
#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include <ZLNetworkRequest.h>
#include <ZLExecutionUtil.h>

#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkErrors.h"
#include "NetworkActions.h"
#include "AuthenticationDialogManager.h"
#include "NetworkOperationRunnable.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

#include "../library/Book.h"
#include "../fbreader/FBReader.h"

NetworkBookReadAction::NetworkBookReadAction(const NetworkBookItem &book, bool demo) : myBook(book), myDemo(demo) {
}

ZLResourceKey NetworkBookReadAction::key() const {
	return ZLResourceKey(myDemo ? "readDemo" : "read");
}

bool NetworkBookReadAction::makesSense() const {
	if (myDemo) {
		if (!myBook.localCopyFileName().empty() ||
				!myBook.reference(BookReference::DOWNLOAD_FULL).isNull()) {
			return false;
		}
		shared_ptr<BookReference> reference =
			myBook.reference(BookReference::DOWNLOAD_DEMO);
		return !reference.isNull() && !reference->localCopyFileName().empty();
	} else {
		return !myBook.localCopyFileName().empty();
	}
}

void NetworkBookReadAction::run() {
	std::string fileName;
	if (myDemo) {
		shared_ptr<BookReference> reference =
			myBook.reference(BookReference::DOWNLOAD_DEMO);
		if (!reference.isNull()) {
			fileName = reference->localCopyFileName();
		}
	} else {
		fileName = myBook.localCopyFileName();
	}
	if (!fileName.empty()) {
		FBReader &fbreader = FBReader::Instance();
		shared_ptr<Book> bookPtr;
		fbreader.createBook(ZLFile(fileName), bookPtr);
		if (!bookPtr.isNull()) {
			fbreader.openBook(bookPtr);
			fbreader.setMode(FBReader::BOOK_TEXT_MODE);
			fbreader.refreshWindow();
			NetworkLibrary::Instance().refresh();
		}
	}
}

NetworkBookDownloadAction::NetworkBookDownloadAction(NetworkBookTree &tree, const NetworkBookItem &book, bool demo, const std::string &tag) : myTree(tree), myBook(book), myDemo(demo), myTag(tag) {
}

ZLResourceKey NetworkBookDownloadAction::key() const {
	return ZLResourceKey(myDemo ? "downloadDemo" : "download");
}

bool NetworkBookDownloadAction::makesSense() const {
	if (myDemo) {
		if (!myBook.localCopyFileName().empty() ||
				!myBook.reference(BookReference::DOWNLOAD_FULL).isNull()) {
			return false;
		}
		shared_ptr<BookReference> reference =
			myBook.reference(BookReference::DOWNLOAD_DEMO);
		return !reference.isNull() && reference->localCopyFileName().empty();
	} else {
		return
			myBook.localCopyFileName().empty() &&
			!myBook.reference(BookReference::DOWNLOAD_FULL).isNull();
	}
}

class NetworkBookDownloadActionListener : public ZLNetworkRequest::Listener {
public:
	NetworkBookDownloadActionListener(NetworkBookDownloadAction *action) : myAction(action) {}
	void finished(const std::string &error) {
		myAction->onBookDownloaded(error);
	}

private:
	NetworkBookDownloadAction *myAction;
};

void NetworkBookDownloadAction::run() {

	myTree.notifyDownloadStarted();

	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	shared_ptr<BookReference> reference = myBook.reference(
		myDemo ? BookReference::DOWNLOAD_DEMO : BookReference::DOWNLOAD_FULL
	);
	if (reference.isNull()) {
		return;
	}
	bool result = NetworkLinkCollection::Instance().downloadBook(*reference, myFileName, new NetworkBookDownloadActionListener(this));
	if (!result) {
		NetworkErrors::showErrorMessage(NetworkLinkCollection::Instance().errorMessage());
	}
}

void NetworkBookDownloadAction::onBookDownloaded(const std::string &error) {

	myTree.notifyDownloadStopped();

	if (!error.empty()) {
		NetworkErrors::showErrorMessage(error);
	}
	FBReader &fbreader = FBReader::Instance();
	shared_ptr<Book> downloaderBook;
	fbreader.createBook(ZLFile(myFileName), downloaderBook);
	if (downloaderBook.isNull()) {
		ZLFile(myFileName).remove();
		ZLResourceKey boxKey("cantOpenDownloadedFile");
		const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
		ZLDialogManager::Instance().errorBox(boxKey, message);
		fbreader.refreshWindow();
		NetworkLibrary::Instance().refresh();
		return;
	}

	downloaderBook->removeAllAuthors();
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = myBook.Authors.begin(); it != myBook.Authors.end(); ++it) {
		downloaderBook->addAuthor(it->DisplayName, it->SortKey);
	}
	std::string bookTitle = myBook.Title;
	if (!myTag.empty()) {
		bookTitle += " (" + myTag + ")";
	}
	downloaderBook->setTitle(bookTitle);
	downloaderBook->setLanguage(myBook.Language);
	for (std::vector<std::string>::const_iterator it = myBook.Tags.begin(); it != myBook.Tags.end(); ++it) {
		downloaderBook->addTag(*it);
	}
	if (!myTag.empty()) {
		downloaderBook->addTag(myTag);
	}
	Library::Instance().addBook(downloaderBook);

	fbreader.openBook(downloaderBook);
	fbreader.setMode(FBReader::BOOK_TEXT_MODE);
	fbreader.refreshWindow();
	NetworkLibrary::Instance().refresh();
}

NetworkBookBuyDirectlyAction::NetworkBookBuyDirectlyAction(NetworkBookTree &tree, const NetworkBookItem &book) :NetworkBookDownloadAction(tree, book, false) {
}

ZLResourceKey NetworkBookBuyDirectlyAction::key() const {
	return ZLResourceKey("buy");
}

bool NetworkBookBuyDirectlyAction::makesSense() const {
	return
		myBook.localCopyFileName().empty() &&
		myBook.reference(BookReference::DOWNLOAD_FULL).isNull() &&
		!myBook.reference(BookReference::BUY).isNull();
}

std::string NetworkBookBuyDirectlyAction::text(const ZLResource &resource) const {
	const std::string text = ZLRunnableWithKey::text(resource);
	shared_ptr<BookReference> reference = myBook.reference(BookReference::BUY);
	if (!reference.isNull()) {
		return ZLStringUtil::printf(text, ((BuyBookReference&)*reference).Price);
	}
	return text;
}

void NetworkBookBuyDirectlyAction::run() {
	if (myBook.Link.authenticationManager().isNull()) {
		finished(std::string());
		return;
	}
	if (!NetworkOperationRunnable::tryConnect()) {
		finished(std::string());
		return;
	}

	NetworkAuthenticationManager &mgr = *myBook.Link.authenticationManager();
	myTree.notifyDownloadStarted();
	mgr.isAuthorised(ZLExecutionUtil::createListener(this, &NetworkBookBuyDirectlyAction::onAuthorisationCheck));
}

class BuyActionAuthListener : public ZLNetworkRequest::Listener {
public:
	BuyActionAuthListener(NetworkBookBuyDirectlyAction &action) : myAction(action) {
	}

	void finished(const std::string &error) {
		myAction.onAuthorised(error);
	}

	void setUIStatus(bool enabled) {
		if (enabled) {
			myAction.myTree.notifyDownloadStarted();
		} else {
			myAction.myTree.notifyDownloadStopped();
		}
	}

private:
	NetworkBookBuyDirectlyAction &myAction;
};

void NetworkBookBuyDirectlyAction::onAuthorisationCheck(ZLUserDataHolder &/*data*/, const std::string &error) {
	myTree.notifyDownloadStopped();
	if (error.empty()) {
		onAuthorised(error);
	} else {
		AuthenticationDialogManager::authAndInitAsync(
			*myBook.Link.authenticationManager(),
			new BuyActionAuthListener(*this)
		);
	}
}

void NetworkBookBuyDirectlyAction::onAuthorised(const std::string &error) {
	if (!error.empty()) {
		finished(std::string()); //ignore error message
		return;
	}
	NetworkAuthenticationManager &mgr = *myBook.Link.authenticationManager();
	if (!mgr.needPurchase(myBook)) {
		finished(std::string());
		return;
	}
	ZLResourceKey boxKey("purchaseConfirmBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
	const int code = ZLDialogManager::Instance().questionBox(boxKey, message, ZLResourceKey("buy"), ZLResourceKey("buyAndDownload"), ZLDialogManager::CANCEL_BUTTON);
	if (code == 2) {
		finished(std::string());
		return;
	}
	bool downloadBook = code == 1;
	if (mgr.needPurchase(myBook)) {
		ZLUserDataHolder *bookData = new ZLUserDataHolder;
		if (downloadBook) {
			bookData->addUserData("downloadBook", new ZLUserData);
		}
		myTree.notifyDownloadStarted();
		mgr.purchaseBook(myBook, ZLExecutionUtil::createListener(bookData, this, &NetworkBookBuyDirectlyAction::onPurchased));
	} else if (downloadBook) {
		NetworkBookDownloadAction::run();
	}
}

void NetworkBookBuyDirectlyAction::onPurchased(ZLUserDataHolder &data, const std::string &error) {
	if (!error.empty()) {
		finished(error);
		return;
	}
	if (data.getUserData("downloadBook").isNull()) {
		finished(std::string());
	} else {
		NetworkBookDownloadAction::run();
	}
}

void NetworkBookBuyDirectlyAction::finished(const std::string &error) {
	myTree.notifyDownloadStopped();
	NetworkLibrary::Instance().refresh();
	if (!error.empty()) {
		ZLDialogManager::Instance().errorBox(ZLResourceKey("networkError"), error);
	}
}

NetworkBookBuyInBrowserAction::NetworkBookBuyInBrowserAction(const NetworkBookItem &book) : myBook(book) {
}

ZLResourceKey NetworkBookBuyInBrowserAction::key() const {
	return ZLResourceKey("buy");
}

bool NetworkBookBuyInBrowserAction::makesSense() const {
	return
		myBook.localCopyFileName().empty() &&
		myBook.reference(BookReference::DOWNLOAD_FULL).isNull() &&
		myBook.reference(BookReference::BUY).isNull() &&
		!myBook.reference(BookReference::BUY_IN_BROWSER).isNull();
}

std::string NetworkBookBuyInBrowserAction::text(const ZLResource &resource) const {
	const std::string text = ZLRunnableWithKey::text(resource);
	shared_ptr<BookReference> reference = myBook.reference(BookReference::BUY_IN_BROWSER);
	if (!reference.isNull()) {
		return ZLStringUtil::printf(text, ((BuyBookReference&)*reference).Price);
	}
	return text;
}

void NetworkBookBuyInBrowserAction::run() {
	shared_ptr<BookReference> reference = myBook.reference(BookReference::BUY_IN_BROWSER);
	if (!reference.isNull()) {
		FBReader::Instance().openLinkInBrowser(reference->URL);
	}
	NetworkLibrary::Instance().refresh();
}

NetworkBookDeleteAction::NetworkBookDeleteAction(const NetworkBookItem &book) : myBook(book) {
}

ZLResourceKey NetworkBookDeleteAction::key() const {
	return ZLResourceKey("delete");
}

bool NetworkBookDeleteAction::makesSense() const {
	return !myBook.localCopyFileName().empty();
}

void NetworkBookDeleteAction::run() {
	ZLResourceKey boxKey("deleteLocalCopyBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}

	myBook.removeLocalFiles();
	FBReader::Instance().refreshWindow();
	NetworkLibrary::Instance().refresh();
}
