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

#include <ZLNetworkManager.h>
#include <ZLDialogManager.h>

#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "../NetworkLinkCollection.h"
#include "../NetworkLink.h"
#include "../NetworkOperationData.h"
#include "../NetworkErrors.h"
#include "NetworkTreeNodes.h"
#include "NetworkLibrary.h"
#include "NetworkSearcher.h"

//TODO rewrite code to not to use fake network link

NetworkSearcher::NetworkSearcher() { }

void NetworkSearcher::simpleSearch(const std::string &pattern) {
	//TODO maybe move code for simple search from NetworkLinkCollection to here
	NetworkCatalogTree *tree = new SearchCatalogTree(&NetworkLibrary::Instance().getFakeCatalogTree(), new AllCatalogsSearchItem(myFakeLink, pattern));
	tree->expand();
}

AllCatalogsSearchItem::AllCatalogsSearchItem(const NetworkLink &link, const std::string &pattern) :
	NetworkCatalogItem(link, std::string(), std::string(), UrlInfoCollection()),
	myPattern(pattern) {
}

class AllCatalogsSearchItemListener : public ZLNetworkRequest::Listener {
public:
	AllCatalogsSearchItemListener(AllCatalogsSearchItem &item,
		NetworkItem::List &children,
		shared_ptr<ZLNetworkRequest::Listener> listener,
		ZLNetworkRequest::Vector requestList,
		std::vector<shared_ptr<NetworkOperationData> > dataList) :
		myItem(item),
		myChildren(children),
		myListener(listener),
		myDataList(dataList),
		myHolder(this) {
		myItemFound = false;
		myCollection = new NetworkBookCollection;
		myCounter = 0;
		performRequests(requestList);
	}

	void performRequests(ZLNetworkRequest::Vector requestList) {
		for (std::size_t i = 0; i < requestList.size(); ++i) {
			shared_ptr<ZLNetworkRequest> request = requestList.at(i);
			request->setListener(myHolder);
			++myCounter;
			ZLNetworkManager::Instance().performAsync(request);
		}
	}

	void finished(const std::string &error) {
		--myCounter;

		ZLNetworkRequest::Vector requestList;

		for (std::size_t i = 0; i < myDataList.size(); ++i) {
			shared_ptr<NetworkOperationData> data = myDataList.at(i);
			for (std::size_t j = 0; j < data->Items.size(); ++j) {
				myCollection->addBook(data->Items.at(j));
			}

			if (!data->Items.empty()) {
				shared_ptr<ZLNetworkRequest> request = data->resume();
				if (!request.isNull()) {
					request->setListener(myHolder);
					requestList.push_back(request);
				}
			}
		}


		if (!myCollection->books().empty()) {
			myItem.onChildrenLoaded(myChildren, myCollection, myListener);
			myChildren.clear();
			myCollection->clear();
		}

		if (!requestList.empty()) {
			performRequests(requestList);
		}

		if (myCounter == 0) {
			myHolder.reset(); //destroy itself
		}
	}

private:
	AllCatalogsSearchItem &myItem;
	NetworkItem::List &myChildren;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
	shared_ptr<NetworkBookCollection> myCollection; //TODO maybe remove this class using

	bool myItemFound;

	std::vector<shared_ptr<NetworkOperationData> > myDataList;
	int myCounter;
//	std::string myErrorMessage;
	shared_ptr<ZLNetworkRequest::Listener> myHolder; //for keeping this instance alive
};

std::string AllCatalogsSearchItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	ZLNetworkRequest::Vector requestList;
	std::vector<shared_ptr<NetworkOperationData> > dataList;

	const NetworkLinkCollection::LinkVector &links = NetworkLinkCollection::Instance().activeLinks();
	for (std::size_t i = 0; i < links.size(); ++i) {
		const NetworkLink &link = *links.at(i);
		shared_ptr<NetworkOperationData> data = new NetworkOperationData(link);

		shared_ptr<ZLNetworkRequest> request = link.simpleSearchData(*data, myPattern);
		if (!request.isNull()) {
			dataList.push_back(data);
			requestList.push_back(request);
		}
	}

	new AllCatalogsSearchItemListener(*this, children, listener, requestList, dataList);

	return std::string();
}

void AllCatalogsSearchItem::onChildrenLoaded(NetworkItem::List &children, shared_ptr<NetworkBookCollection> collection, shared_ptr<ZLNetworkRequest::Listener> listener) {
	if (!collection.isNull()) {
		const NetworkItem::List &books = collection->books();
		children.assign(books.begin(), books.end());
	}
	listener->finished();
}

FakeNetworkLink::FakeNetworkLink() : NetworkLink("") { }

shared_ptr<ZLNetworkRequest> FakeNetworkLink::simpleSearchData(NetworkOperationData &/*data*/, const std::string &/*pattern*/) const {
	return 0;
}

shared_ptr<ZLNetworkRequest> FakeNetworkLink::advancedSearchData(NetworkOperationData &/*data*/, const std::string &/*titleAndSeries*/, const std::string &/*author*/, const std::string &/*tag*/, const std::string &/*annotation*/) const {
	return 0;
}

shared_ptr<ZLNetworkRequest> FakeNetworkLink::resume(NetworkOperationData &/*data*/) const {
	return 0;
}

shared_ptr<NetworkAuthenticationManager> FakeNetworkLink::authenticationManager() const {
	return 0;
}

shared_ptr<NetworkItem> FakeNetworkLink::libraryItem() const {
	return 0;
}

void FakeNetworkLink::rewriteUrl(std::string &/*url*/, bool /*isUrlExternal*/) const {
}
