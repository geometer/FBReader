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

#include <algorithm>

#include <ZLNetworkManager.h>
#include <ZLNetworkUtil.h>
#include <ZLStringUtil.h>

#include "../NetworkLink.h"
#include "../NetworkComparators.h"
#include "../NetworkErrors.h"
#include "../NetworkItems.h"

#include "LitResUtil.h"
#include "LitResBooksFeedParser.h"

#include "LitResBooksFeedItem.h"

LitResBooksFeedItem::LitResBooksFeedItem(
	bool shouldSort,
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility,
	int flags
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	accessibility,
	flags
), myShouldSort(shouldSort) {

}

void LitResBooksFeedItem::onDisplayItem() {
}

class LitResBooksFeedItemRunnable : public ZLNetworkRequest::Listener {
public:
	LitResBooksFeedItemRunnable(LitResBooksFeedItem &item, shared_ptr<ZLNetworkRequest> request, NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) :
		myItem(item), myChildren(children), myListener(listener) {
		request->setListener(this);
		ZLNetworkManager::Instance().performAsync(request);
	}

	void finished(const std::string &error) {
		if (error.empty()) {
			++myItem.myLoadingState.CurrentPage;
			if (myItem.myShouldSort) {
				std::sort(myChildren.begin(), myChildren.end(), NetworkBookItemComparator());
			}
		}
		myListener->finished(error);
	}
private:
	LitResBooksFeedItem &myItem;
	NetworkItem::List &myChildren;
	shared_ptr<ZLNetworkRequest::Listener> myListener;
};


std::string LitResBooksFeedItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
//	//TODO maybe add sid parameter if possible
//	//(at LitRes API documentation it said that's adding sid _always_ is a good practice)

	myLoadingState.CurrentPage = 0;
	myLoadingState.AllPagesCount = 1;

	shared_ptr<ZLNetworkRequest> request = getRequest(children);
	new LitResBooksFeedItemRunnable(*this, request, children, listener);
	return std::string();
}

bool LitResBooksFeedItem::supportsResumeLoading() {
	return true;
}

std::string LitResBooksFeedItem::resumeLoading(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	shared_ptr<ZLNetworkRequest> request = getRequest(children);
	if (request.isNull()) {
		listener->finished();
		return std::string();
	}
	new LitResBooksFeedItemRunnable(*this, request, children, listener);
	return std::string();
}

shared_ptr<ZLNetworkRequest> LitResBooksFeedItem::getRequest(NetworkItem::List &children) {
	if (myLoadingState.CurrentPage >= myLoadingState.AllPagesCount) {
		return 0;
	}
	return ZLNetworkManager::Instance().createXMLParserRequest(
		withLimitParameters(getCatalogUrl(), myLoadingState),
		new LitResBooksFeedParser(Link, children, &myLoadingState)
	);
}

std::string LitResBooksFeedItem::withLimitParameters(std::string query, const LoadingState &state) {
	static const unsigned int ITEMS_PER_PAGE = 40;
	unsigned int startItemNumber = (unsigned int)state.CurrentPage * ITEMS_PER_PAGE;
	std::string params;
	ZLStringUtil::appendNumber(params, startItemNumber);
	params += ",";
	ZLStringUtil::appendNumber(params, ITEMS_PER_PAGE);
	ZLNetworkUtil::appendParameter(query, "limit", params);
	return query;
}

