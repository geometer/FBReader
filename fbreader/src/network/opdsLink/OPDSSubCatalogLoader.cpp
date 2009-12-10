/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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
#include <ZLNetworkXMLParserData.h>

#include "OPDSSubCatalogLoader.h"
#include "OPDSLink.h"

#include "../NetworkOperationData.h"
#include "../opds/OPDSXMLParser.h"
#include "../opds/NetworkOPDSFeedReader.h"

shared_ptr<NetworkSubCatalogLoader> OPDSSubCatalogLoader::ourInstance;

shared_ptr<NetworkSubCatalogLoader> OPDSSubCatalogLoader::Instance() {
	if (ourInstance.isNull()) {
		ourInstance = new OPDSSubCatalogLoader();
	}
	return ourInstance;
}

OPDSSubCatalogLoader::OPDSSubCatalogLoader() {
}

std::string OPDSSubCatalogLoader::load(NetworkLibraryCatalogItem &item, NetworkLibraryItemList &children) {
	NetworkOperationData data(item.link());

	shared_ptr<ZLExecutionData> networkData =
		OPDSLink::createNetworkData(item.url(), data);

	while (!networkData.isNull()) {
		ZLExecutionData::Vector dataList;
		dataList.push_back(networkData);
		std::string error = ZLNetworkManager::Instance().perform(dataList);
		if (!error.empty()) {
			return error;
		}

		children.insert(children.end(), data.Items.begin(), data.Items.end());
		networkData = item.link().resume(data);
	}

	return "";
}
