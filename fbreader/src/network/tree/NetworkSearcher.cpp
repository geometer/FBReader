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

#include "../authentication/NetworkAuthenticationManager.h"
#include "../../networkActions/NetworkOperationRunnable.h"
#include "../NetworkLinkCollection.h"
#include "../NetworkLink.h"
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

std::string AllCatalogsSearchItem::loadChildren(NetworkItem::List &children) {
	SimpleSearchRunnable runnable(myPattern);
	runnable.executeWithUI();
	runnable.showErrorMessage();

	shared_ptr<NetworkBookCollection> result = runnable.result();
	if (!result.isNull()) {
		const NetworkItem::List &books = result->books();
		children.assign(books.begin(), books.end());
	}
	return std::string();
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
