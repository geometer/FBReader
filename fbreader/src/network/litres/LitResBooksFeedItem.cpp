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

std::string LitResBooksFeedItem::loadChildren(NetworkItem::List &children) {
	//TODO maybe add sid parameter if possible
	//(at LitRes API documentation it said that's adding sid _always_ is a good practice)
	shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createXMLParserRequest(
		getCatalogUrl(),
		new LitResBooksFeedParser(Link, children)
	);
	std::string error = ZLNetworkManager::Instance().perform(data);
	if (myShouldSort) {
		std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	}
	return error;
}
