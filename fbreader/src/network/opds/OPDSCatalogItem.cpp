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

#include <ZLNetworkRequest.h>
#include <ZLNetworkManager.h>

#include "OPDSCatalogItem.h"
#include "OPDSLink.h"
#include "OPDSXMLParser.h"
#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"

OPDSCatalogItem::OPDSCatalogItem(
	const OPDSLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility,
	int flags
		) : NetworkCatalogItem(link, title, summary, urlByType, accessibility, flags), myData(Link) {
}

//class OPDSCatalogItemScope : public ZLExecutionScope {
//public:
//	shared_ptr<ZLNetworkRequest> networkData;
//	shared_ptr<ZLNetworkRequest::Listener> listener;
//};

class OPDSCatalogItemRunnable : public ZLRunnable {
public:
	OPDSCatalogItemRunnable(NetworkItem::List &children, NetworkOperationData &data) : myChildren(children), myData(data) {}
	void run() {
		myChildren.insert(myChildren.end(), myData.Items.begin(), myData.Items.end());
	}
private:
	NetworkItem::List &myChildren;
	NetworkOperationData &myData;
};

std::string OPDSCatalogItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {

	//TODO implement partial catalogs loading
	myData.clear();

	shared_ptr<ZLNetworkRequest> networkData = ((OPDSLink&)Link).createNetworkData(getCatalogUrl(), myData, new OPDSCatalogItemRunnable(children, myData));
	networkData->setListener(listener);
	return ZLNetworkManager::Instance().performAsync(networkData);
}
