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

#include "RSSLink.h"

#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

RSSLink::RSSLink(const std::string &siteName) : OPDSLink(siteName){
}

RSSLink::~RSSLink() {
}

shared_ptr<NetworkItem> RSSLink::libraryItem() const {
    NetworkItem::UrlInfoCollection urlMap;
    urlMap[NetworkItem::URL_COVER] = getIcon();
    urlMap[NetworkItem::URL_CATALOG] = url(URL_MAIN);
    return new RSSCatalogItem(*this, getTitle(), getSummary(), urlMap);
}

shared_ptr<ZLNetworkRequest> RSSLink::createNetworkRequest(const std::string &url, NetworkOperationData &result) const{
    if (url.empty()) {
        return 0;
    }
    return ZLNetworkManager::Instance().createXMLParserRequest(url, new RSSXMLParser(new NetworkRSSChannelReader(*this, url, result)));
}
