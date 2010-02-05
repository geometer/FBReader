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

#include <ZLExecutionData.h>
#include <ZLNetworkManager.h>

#include "OPDSCatalogItem.h"
#include "OPDSLink.h"

#include "../NetworkOperationData.h"
#include "../opds/OPDSXMLParser.h"
#include "../opds/NetworkOPDSFeedReader.h"

OPDSCatalogItem::OPDSCatalogItem(
	OPDSLink &link,
	const std::string &url,
	const std::string &htmlURL,
	const std::string &title,
	const std::string &summary,
	const std::string &coverURL,
	bool dependsOnAccount
) : NetworkLibraryCatalogItem(link, url, htmlURL, title, summary, coverURL, dependsOnAccount) {
}

std::string OPDSCatalogItem::loadChildren(NetworkLibraryItemList &children) {
	NetworkOperationData data(link());

	shared_ptr<ZLExecutionData> networkData =
		((OPDSLink &)link()).createNetworkData(url(), data);

	while (!networkData.isNull()) {
		std::string error = ZLNetworkManager::Instance().perform(networkData);
		if (!error.empty()) {
			return error;
		}

		children.insert(children.end(), data.Items.begin(), data.Items.end());
		networkData = link().resume(data);
	}

	return "";
}
