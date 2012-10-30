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

#ifndef __NETWORKSEARCHER_H__
#define __NETWORKSEARCHER_H__

#include <ZLTreeSearcher.h>

#include "../NetworkBookCollection.h"
#include "../NetworkItems.h"

//TODO rewrite code to not to use fake network link
class FakeNetworkLink : public NetworkLink {
public:
	FakeNetworkLink();
	shared_ptr<ZLNetworkRequest> simpleSearchData(NetworkOperationData &data, const std::string &pattern) const;
	shared_ptr<ZLNetworkRequest> advancedSearchData(NetworkOperationData &data, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) const;
	shared_ptr<ZLNetworkRequest> resume(NetworkOperationData &data) const;
	shared_ptr<NetworkAuthenticationManager> authenticationManager() const;
	shared_ptr<NetworkItem> libraryItem() const;
	void rewriteUrl(std::string &url, bool isUrlExternal = false) const;
};

class NetworkSearcher : public ZLTreeSearcher {
public:
    NetworkSearcher();
	void simpleSearch(const std::string &pattern);
private:
	const FakeNetworkLink myFakeLink;
};

class AllCatalogsSearchItem : public NetworkCatalogItem {

public:
	AllCatalogsSearchItem(const NetworkLink &link, const std::string &pattern);
	std::string loadChildren(List &children);

private:
	std::string myPattern;
};

#endif /* __NETWORKSEARCHER_H__ */
