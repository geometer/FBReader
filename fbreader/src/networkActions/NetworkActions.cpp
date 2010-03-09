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
#include <ZLFile.h>
#include <ZLStringUtil.h>
#include <ZLDialogManager.h>

#include "NetworkActions.h"
#include "AuthenticationDialog.h"
#include "NetworkOperationRunnable.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLink.h"
#include "../network/authentication/NetworkAuthenticationManager.h"

#include "../library/Book.h"
#include "../fbreader/FBReader.h"

static const std::string DEMO_SUFFIX = " (фрагмент)";
static const std::string DEMO_TAG = "Фрагмент";

NetworkBookReadAction::NetworkBookReadAction(const NetworkBookItem &book, bool demo) : myBook(book), myDemo(demo) {
}

ZLResourceKey NetworkBookReadAction::key() const {
	return ZLResourceKey(myDemo ? "readDemo" : "read");
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
		fbreader.createBook(fileName, bookPtr);
		if (!bookPtr.isNull()) {
			fbreader.openBook(bookPtr);
			fbreader.setMode(FBReader::BOOK_TEXT_MODE);
			fbreader.refreshWindow();
		}
	}
}

NetworkBookDownloadAction::NetworkBookDownloadAction(const NetworkBookItem &book, bool demo) : myBook(book), myDemo(demo) {
}

ZLResourceKey NetworkBookDownloadAction::key() const {
	return ZLResourceKey(myDemo ? "downloadDemo" : "download");
}

void NetworkBookDownloadAction::run() {
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	shared_ptr<BookReference> reference = myBook.reference(
		myDemo ? BookReference::DOWNLOAD_DEMO : BookReference::DOWNLOAD
	);
	if (reference.isNull()) {
		return;
	}

	DownloadBookRunnable downloader(reference, myBook.Link.authenticationManager());
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
		const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
		ZLDialogManager::Instance().errorBox(boxKey, message);
		fbreader.refreshWindow();
		return;
	}

	downloaderBook->removeAllAuthors();
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = myBook.Authors.begin(); it != myBook.Authors.end(); ++it) {
		downloaderBook->addAuthor(it->DisplayName, it->SortKey);
	}
	downloaderBook->setTitle(myDemo ? myBook.Title + DEMO_SUFFIX : myBook.Title);
	downloaderBook->setLanguage(myBook.Language);
	for (std::vector<std::string>::const_iterator it = myBook.Tags.begin(); it != myBook.Tags.end(); ++it) {
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

NetworkBookBuyAction::NetworkBookBuyAction(const NetworkBookItem &book) : myBook(book) {
}

ZLResourceKey NetworkBookBuyAction::key() const {
	return ZLResourceKey("buy");
}

std::string NetworkBookBuyAction::text(const ZLResource &resource) const {
	const std::string text = ZLRunnableWithKey::text(resource);
	shared_ptr<BookReference> reference = myBook.reference(BookReference::BUY);
	if (!reference.isNull()) {
		return ZLStringUtil::printf(text, ((BuyBookReference&)*reference).Price);
	}
	return text;
}

void NetworkBookBuyAction::run() {
	FBReader &fbreader = FBReader::Instance();
	if (myBook.Link.authenticationManager().isNull()) {
		return;
	}
	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}
	NetworkAuthenticationManager &mgr = *myBook.Link.authenticationManager();
	if (mgr.isAuthorised().Status != B3_TRUE) {
		if (!AuthenticationDialog::run(mgr)) {
			return;
		}
		fbreader.invalidateAccountDependents();
		fbreader.refreshWindow();
		if (!mgr.needPurchase(myBook)) {
			return;
		}
	}
	ZLResourceKey boxKey("purchaseConfirmBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
	const int code = ZLDialogManager::Instance().questionBox(boxKey, message, ZLResourceKey("buy"), ZLResourceKey("buyAndDownload"), ZLDialogManager::CANCEL_BUTTON);
	if (code == 2) {
		return;
	}
	bool downloadBook = code == 1;
	if (mgr.needPurchase(myBook)) {
		PurchaseBookRunnable purchaser(mgr, myBook);
		purchaser.executeWithUI();
		if (purchaser.hasErrors()) {
			purchaser.showErrorMessage();
			downloadBook = false;
		}
	}
	if (downloadBook) {
		NetworkBookDownloadAction(myBook, false).run();
	}
	if (mgr.isAuthorised().Status == B3_FALSE) {
		fbreader.invalidateAccountDependents();
	}
	fbreader.refreshWindow();
}

NetworkBookDeleteAction::NetworkBookDeleteAction(const NetworkBookItem &book) : myBook(book) {
}

ZLResourceKey NetworkBookDeleteAction::key() const {
	return ZLResourceKey("delete");
}

void NetworkBookDeleteAction::run() {
	ZLResourceKey boxKey("deleteLocalCopyBox");
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), myBook.Title);
	if (ZLDialogManager::Instance().questionBox(boxKey, message, ZLDialogManager::YES_BUTTON, ZLDialogManager::NO_BUTTON) != 0) {
		return;
	}

	myBook.removeLocalFiles();
	FBReader::Instance().refreshWindow();
}
