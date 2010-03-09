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
#include <ZLExecutionData.h>
#include <ZLTimeManager.h>

#include "NetworkView.h"
#include "NetworkNodes.h"
#include "NetworkNodesFactory.h"

#include "../network/NetworkItems.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/NetworkLink.h"
#include "../network/SearchResult.h"
#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../networkActions/NetworkOperationRunnable.h"

#include "../options/FBOptions.h"

#include "../fbreader/FBReader.h"

NetworkView::NetworkView(ZLPaintContext &context) : ZLBlockTreeView(context), myUpdateChildren(true), myUpdateAccountDependents(false) {
}

void NetworkView::drawCoverLater(FBReaderNode *node, int vOffset) {
	myNodesToPaintCovers[node] = vOffset;
}

class NetworkView::CoverUpdater : public NetworkOperationRunnable {

private:
	static volatile int Counter;

public:
	static shared_ptr<CoverUpdater> create();

private:
	CoverUpdater();

public:
	~CoverUpdater();

	void addTask(shared_ptr<ZLExecutionData> data);
	bool hasTasks() const;

private:
	void run();

private:
	ZLExecutionData::Vector myDataVector;
};

class NetworkView::CoverUpdaterRunner : public ZLRunnable {

public:
	CoverUpdaterRunner(shared_ptr<CoverUpdater> updater);

private:
	void run();

private:
	shared_ptr<CoverUpdater> myUpdater;
};

volatile int NetworkView::CoverUpdater::Counter = 0;

shared_ptr<NetworkView::CoverUpdater> NetworkView::CoverUpdater::create() {
	return Counter == 0 ? new CoverUpdater() : 0;
}

NetworkView::CoverUpdater::CoverUpdater() : NetworkOperationRunnable("downloadImages") {
	++Counter;
}

NetworkView::CoverUpdater::~CoverUpdater() {
	--Counter;
}

void NetworkView::CoverUpdater::addTask(shared_ptr<ZLExecutionData> data) {
	if (!data.isNull()) {
		myDataVector.push_back(data);
	}
}

bool NetworkView::CoverUpdater::hasTasks() const {
	return !myDataVector.empty();
}

void NetworkView::CoverUpdater::run() {
	ZLExecutionData::perform(myDataVector);
}

NetworkView::CoverUpdaterRunner::CoverUpdaterRunner(shared_ptr<CoverUpdater> updater) : myUpdater(updater) {
}

void NetworkView::CoverUpdaterRunner::run() {
	if (myUpdater->hasTasks()) {
		myUpdater->executeWithUI();
		FBReader::Instance().refreshWindow();
	}
}

void NetworkView::paint() {
	if (myUpdateChildren) {
		myUpdateChildren = false;
		makeUpToDate();
	}
	if (myUpdateAccountDependents) {
		myUpdateAccountDependents = false;
		updateAccountDependents();
	}
	myNodesToPaintCovers.clear();
	ZLBlockTreeView::paint();
	std::map<FBReaderNode*,int> nodes;
	nodes.swap(myNodesToPaintCovers);
	shared_ptr<CoverUpdater> updater = CoverUpdater::create();
	if (!updater.isNull()) {
		for (std::map<FBReaderNode*,int>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
			shared_ptr<ZLImage> coverImage = it->first->coverImage();
			if (!coverImage.isNull()) {
				updater->addTask(coverImage->synchronizationData());
			}
		}
		if (updater->hasTasks()) {
			ZLTimeManager::Instance().addAutoRemovableTask(new CoverUpdaterRunner(updater));
		}
	}
	for (std::map<FBReaderNode*,int>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		it->first->drawCoverReal(context(), it->second);
	}
	myNodesToPaintCovers.clear();
}

const std::string &NetworkView::caption() const {
	return ZLResource::resource("networkLibrary")["caption"].value();
}

ZLColor NetworkView::backgroundColor() const {
	return FBOptions::Instance().BackgroundColorOption.value();
}

void NetworkView::makeUpToDate() {
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();

	std::set<ZLBlockTreeNode*> nodesToDelete;
	ZLBlockTreeNode::List rootChildren = rootNode().children();

	ZLBlockTreeNode::List::iterator nodeIt = rootChildren.begin();
	size_t nodeCount = 0;
	for (size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
		if (!link.OnOption.value()) {
			continue;
		}
		bool processed = false;
		while (nodeIt != rootChildren.end() &&
					 (*nodeIt)->isInstanceOf(NetworkCatalogNode::TYPE_ID)) {
			const NetworkLink &nodeLink = ((NetworkCatalogRootNode*)*nodeIt)->link();
			if (&nodeLink == &link) {
				++nodeIt;
				++nodeCount;
				processed = true;
				break;
			} else {
				bool found = false;
				for (size_t j = i + 1; j < collection.size(); ++j) {
					if (&nodeLink == &collection.link(j)) {
						found = true;
						break;
					}
				}
				if (!found) {
					nodesToDelete.insert(*nodeIt++);
					++nodeCount;
				} else {
					break;
				}
			}
		}
		if (!processed) {
			NetworkCatalogNode *ptr = new NetworkCatalogRootNode(&rootNode(), link, nodeCount++);
			ptr->item().onDisplayItem();
		}
	}

	SearchResultNode *srNode = 0;

	while (nodeIt != rootChildren.end()) {
		ZLBlockTreeNode *node = *nodeIt++;
		++nodeCount;
		if (node->isInstanceOf(SearchResultNode::TYPE_ID)) {
			srNode = (SearchResultNode*)node;
		} else {
			nodesToDelete.insert(node);
		}
	}

	const SearchResult &searchResult = SearchResult::lastSearchResult();
	shared_ptr<NetworkBookCollection> result = searchResult.collection();
	if (result.isNull()) {
		if (srNode != 0) {
			nodesToDelete.insert(srNode);
		}
	} else if (srNode == 0 || srNode->searchResult() != result) {
		if (srNode != 0) {
			nodesToDelete.insert(srNode);
		}
		srNode = new SearchResultNode(&rootNode(), result, searchResult.summary()); // at nodeCount ??? or not???
		NetworkNodesFactory::createSubnodes(srNode, *result);
	}

	for (std::set<ZLBlockTreeNode*>::iterator it = nodesToDelete.begin(); it != nodesToDelete.end(); ++it) {
		delete *it;
	}

	if (srNode != 0) {
		srNode->open(false);
		srNode->expandOrCollapseSubtree();
	}
}

void NetworkView::updateAccountDependents() {
	ZLBlockTreeNode::List rootChildren = rootNode().children();

	ZLBlockTreeNode::List::iterator nodeIt = rootChildren.begin();
	while (nodeIt != rootChildren.end() && (*nodeIt)->isInstanceOf(NetworkCatalogNode::TYPE_ID)) {
		NetworkCatalogNode &node = (NetworkCatalogNode &) **nodeIt;
		updateAccountDependents(node);
		++nodeIt;
	}
}

void NetworkView::updateAccountDependents(NetworkCatalogNode &node) {
	std::set<ZLBlockTreeNode*> nodesToDelete;

	const NetworkItem::List &nodeItems = node.childrenItems();

	ZLBlockTreeNode::List nodeChildren = node.children();
	ZLBlockTreeNode::List::iterator nodeIt = nodeChildren.begin();

	size_t nodeCount = 0;
	for (size_t i = 0; i < nodeItems.size(); ++i) {
		shared_ptr<NetworkItem> currentItemPtr = nodeItems[i];
		NetworkItem &currentItem = *currentItemPtr;

		if (currentItem.typeId() != NetworkCatalogItem::TYPE_ID) {
			continue;
		}

		bool processed = false;
		while (nodeIt != nodeChildren.end()) {
			if (!(*nodeIt)->isInstanceOf(NetworkCatalogNode::TYPE_ID)) {
				++nodeIt;
				++nodeCount;
				continue;
			}
			NetworkCatalogNode &child = (NetworkCatalogNode &) **nodeIt;
			NetworkCatalogItem &childItem = child.item();
			if (&childItem == &currentItem) {
				if (processAccountDependent(child.item())) {
					updateAccountDependents(child);
				} else {
					nodesToDelete.insert(&child);
				}
				++nodeIt;
				++nodeCount;
				processed = true;
				break;
			} else {
				bool found = false;
				for (size_t j = i + 1; j < nodeItems.size(); ++j) {
					if (&childItem == &*nodeItems[j]) {
						found = true;
						break;
					}
				}
				if (!found) {
					nodesToDelete.insert(*nodeIt++);
					++nodeCount;
				} else {
					break;
				}
			}
		}
		if (!processed && processAccountDependent((NetworkCatalogItem &) currentItem)) {
			NetworkNodesFactory::createNetworkNode(&node, currentItemPtr, nodeCount++);
		}
	}

	while (nodeIt != nodeChildren.end()) {
		ZLBlockTreeNode *node = *nodeIt++;
		if (node->isInstanceOf(NetworkCatalogNode::TYPE_ID)) {
			nodesToDelete.insert(node);
		}
	}

	for (std::set<ZLBlockTreeNode*>::iterator it = nodesToDelete.begin(); it != nodesToDelete.end(); ++it) {
		delete *it;
	}
}

bool NetworkView::processAccountDependent(NetworkCatalogItem &item) {
	if (item.Visibility == NetworkCatalogItem::Always) {
		return true;
	}
	const NetworkLink &link = item.Link;
	if (link.authenticationManager().isNull()) {
		return false;
	}
	return link.authenticationManager()->isAuthorised().Status != B3_FALSE;
}
