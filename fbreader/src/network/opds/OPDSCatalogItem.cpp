/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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
		) : NetworkCatalogItem(link, title, summary, urlByType, accessibility, flags), myLoadingState(Link) {
}

class OPDSCatalogItemRunnable : public ZLNetworkRequest::Listener {
public:
	OPDSCatalogItemRunnable(shared_ptr<ZLNetworkRequest> request, NetworkItem::List &children, NetworkOperationData &data, shared_ptr<ZLNetworkRequest::Listener> listener) :
		myChildren(children), myLoadingData(data), myListener(listener) {
		request->setListener(this);
		ZLNetworkManager::Instance().performAsync(request);
	}
	void finished(const std::string &error) {
		myChildren.insert(myChildren.end(), myLoadingData.Items.begin(), myLoadingData.Items.end());
		myListener->finished(error);
	}
	void setUIStatus(bool enabled) {
		myListener->setUIStatus(enabled); //to hide refreshing while authentication dialog
	}

private:
	NetworkItem::List &myChildren;
	NetworkOperationData &myLoadingData;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
};


std::string OPDSCatalogItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	myLoadingState.clear();
	shared_ptr<ZLNetworkRequest> request = ((OPDSLink&)Link).createNetworkRequest(getCatalogUrl(), myLoadingState);
	new OPDSCatalogItemRunnable(request, children, myLoadingState, listener);
	return std::string();
}

bool OPDSCatalogItem::supportsResumeLoading() {
	return true;
}

std::string OPDSCatalogItem::resumeLoading(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	shared_ptr<ZLNetworkRequest> request = myLoadingState.resume();
	if (request.isNull()) {
		listener->finished();
		return std::string();
	}
	new OPDSCatalogItemRunnable(request, children, myLoadingState, listener);
	return std::string();
}
