/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkTreeNodes.h"
#include "NetworkTreeFactory.h"
#include "NetworkLibrary.h"
#include "../../networkTree/NetworkCatalogUtil.h"

#include "../../networkActions/NetworkActions.h"

const ZLTypeId NetworkBookTree::TYPE_ID(ZLTreePageNode::TYPE_ID);

const ZLTypeId &NetworkBookTree::typeId() const {
	return TYPE_ID;
}

const ZLResource &NetworkBookTree::resource() const {
	return ZLResource::resource("networkView")["bookNode"];
}

NetworkBookTree::NetworkBookTree(NetworkTree *parent, shared_ptr<NetworkItem> book, SummaryType summaryType) : ZLTreePageNode(parent), myBook(book), mySummaryType(summaryType) {
	init();
}

class NetworkTreeBookReadAction : public NetworkBookReadAction {
public:
	NetworkTreeBookReadAction(const NetworkBookTree &tree, const NetworkBookItem &book, bool demo) : NetworkBookReadAction(book, demo), myTree(tree) {}
	void run() {
		NetworkBookReadAction::run();
		myTree.close();
	}

private:
	const NetworkBookTree &myTree;
};

class NetworkTreeBookDownloadAction : public NetworkBookDownloadAction {
public:
	NetworkTreeBookDownloadAction(NetworkBookTree &tree, const NetworkBookItem &book, bool demo, const std::string &tag = std::string()) :
		NetworkBookDownloadAction(book, demo, tag), myTree(tree) {}

	void run() {
		myTree.notifyDownloadStarted();
		NetworkBookDownloadAction::run();
	}

	void finished(const std::string &error) {
		myTree.notifyDownloadStopped();
		NetworkBookDownloadAction::finished(error);
	}

private:
	NetworkBookTree &myTree;
};

static std::vector<shared_ptr<ZLTreeAction> > getBookActions(NetworkBookTree &tree) {
	std::vector<shared_ptr<ZLTreeAction> > actions;
	const NetworkBookItem &book = tree.book();
	if (!book.reference(BookReference::DOWNLOAD_FULL).isNull() ||
			!book.reference(BookReference::DOWNLOAD_FULL_CONDITIONAL).isNull()) {
		actions.push_back(new NetworkTreeBookReadAction(tree, book, false));
		actions.push_back(new NetworkTreeBookDownloadAction(tree, book, false));
		actions.push_back(new NetworkBookDeleteAction(book));
	}
	if (!book.reference(BookReference::DOWNLOAD_DEMO).isNull()) {
		actions.push_back(new NetworkTreeBookReadAction(tree, book, true));
		actions.push_back(new NetworkTreeBookDownloadAction(tree, book, true, tree.resource()["demo"].value()));
	}
	if (!book.reference(BookReference::BUY).isNull()) {
		actions.push_back(new NetworkBookBuyDirectlyAction(book));
	} else if (!book.reference(BookReference::BUY_IN_BROWSER).isNull()) {
		actions.push_back(new NetworkBookBuyInBrowserAction(book));
	}
	return actions;
}

void NetworkBookTree::init() {
	std::vector<shared_ptr<ZLTreeAction> > actions = getBookActions(*this);
	for (size_t i = 0; i < actions.size(); ++i) {
		registerAction(actions.at(i));
	}
}

std::string NetworkBookTree::title() const {
	return myBook->Title;
}

std::string NetworkBookTree::subtitle() const {
	int count = 0;
	std::string authorsString;
	const std::vector<NetworkBookItem::AuthorData> authors = book().Authors;
	for (std::vector<NetworkBookItem::AuthorData>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
		if (!authorsString.empty()) {
			authorsString += ", ";
		}
		authorsString += it->DisplayName;
		++count;
	}
	if (mySummaryType == NONE && count == 1) {
		return std::string();
	}
	return authorsString;
}

shared_ptr<ZLTreePageInfo> NetworkBookTree::getPageInfo() /*const*/ {
	//WARNING: using this, we have a potenital problems with controlling NetworkBookTree object's life time in memory
	return new BookItemWrapper(*this, myBook);
}


shared_ptr<const ZLImage> NetworkBookTree::image() const {
	shared_ptr<const ZLImage> image = NetworkCatalogUtil::getAndDownloadImageByUrl(myBook->URLByType[NetworkItem::URL_COVER], this);
	return !image.isNull() ? image : FBTree::defaultCoverImage("booktree-book.png");
}

shared_ptr<const ZLImage> NetworkBookTree::fullImage() const {
	if (myBook->URLByType.find(NetworkItem::URL_FULL_COVER) == myBook->URLByType.end()) {
		return 0;
	}
	shared_ptr<const ZLImage> fullImage = NetworkCatalogUtil::getImageByUrl(myBook->URLByType[NetworkItem::URL_FULL_COVER]);
	return !fullImage.isNull() ? fullImage : 0;
}

const NetworkBookItem &NetworkBookTree::book() const {
	return (const NetworkBookItem&)*myBook;
}


NetworkBookTree::BookItemWrapper::BookItemWrapper(NetworkBookTree &tree, shared_ptr<NetworkItem> bookItem) : myTree(tree), myBookItem(bookItem), myIsInitialized(false) {
}

std::string NetworkBookTree::BookItemWrapper::title() const {
	initialize();
	return book().Title;
}

std::vector<std::string> NetworkBookTree::BookItemWrapper::authors() const {
	initialize();
	const NetworkBookItem &bookItem = book();
	std::vector<std::string> authors;
	for (size_t i = 0; i < bookItem.Authors.size(); ++i) {
		authors.push_back(bookItem.Authors.at(i).DisplayName);
	}
	return authors;
}

std::vector<std::string> NetworkBookTree::BookItemWrapper::tags() const {
	initialize();
	return book().Tags;
}

std::string NetworkBookTree::BookItemWrapper::summary() const {
	initialize();
	return book().Summary;
}

shared_ptr<const ZLImage> NetworkBookTree::BookItemWrapper::image() const {
	initialize();
	shared_ptr<const ZLImage> fullImage = myTree.fullImage();
	if (!fullImage.isNull()) {
		return fullImage;
	}
	return myTree.image();
}

const std::vector<shared_ptr<ZLTreeAction> > &NetworkBookTree::BookItemWrapper::actions() const {
	return myActions;
}

std::string NetworkBookTree::BookItemWrapper::actionText(const shared_ptr<ZLTreeAction> &action) const {
	return myTree.actionText(action);
}

class RelatedAction : public ZLTreeAction {
public:
	RelatedAction(shared_ptr<NetworkItem> item) : myTitle(item->Title) {
		myNode = new NetworkCatalogTree(&NetworkLibrary::Instance().getFakeCatalogTree(), item);
		//myNode = NetworkTreeFactory::createNetworkTree(0, item);
	}
	ZLResourceKey key() const { return ZLResourceKey(""); }
	std::string text(const ZLResource &/*resource*/) const {
		return myTitle;
	}
	void run() {
		if (NetworkCatalogTree *tree = zlobject_cast<NetworkCatalogTree*>(&*myNode)){
			tree->expand();
		}
	}

private:
	shared_ptr<ZLTreeTitledNode> myNode;
	std::string myTitle;
};

const std::vector<shared_ptr<ZLTreeAction> > NetworkBookTree::BookItemWrapper::relatedActions() const {
	if (!myRelatedActions.empty()) {
		return myRelatedActions;
	}
	std::vector<shared_ptr<NetworkItem> > catalogItems =  static_cast<NetworkBookItem&>(*myBookItem).getRelatedCatalogsItems();
	for (size_t i = 0; i < catalogItems.size(); ++i) {
		shared_ptr<NetworkItem> item = catalogItems.at(i);
		myRelatedActions.push_back(new RelatedAction(item));
	}
	return myRelatedActions;
}

void NetworkBookTree::BookItemWrapper::initialize() const {
	if (myIsInitialized) {
		return;
	}
	NetworkBookItem &bookItem = book();
	if (!bookItem.isFullyLoaded()) {
		bookItem.loadFullInformation();
	}

	myActions = getBookActions(myTree);
	myIsInitialized = true;
}

NetworkBookItem &NetworkBookTree::BookItemWrapper::book() const {
   return (NetworkBookItem&)*myBookItem;
}
