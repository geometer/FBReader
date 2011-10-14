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

#include <algorithm>

#include <ZLExecutionData.h>
#include <ZLTimeManager.h>

#include "LitResBookshelfItem.h"
#include "LitResAuthenticationManager.h"

#include "../../NetworkLink.h"
#include "../../NetworkComparators.h"
#include "../../NetworkErrors.h"

LitResBookshelfItem::LitResBookshelfItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const std::map<URLType,std::string> &urlByType,
	VisibilityType visibility
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	visibility
) {
	myForceReload = false;
}

void LitResBookshelfItem::onDisplayItem() {
	myForceReload = false;
}

class LitResBookshelfItemLoaderScope : public ZLUserData {
public:
	NetworkItem::List *children;
	shared_ptr<ZLExecutionData::Listener> listener;
};

std::string LitResBookshelfItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener) {
	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*Link.authenticationManager());
	shared_ptr<ZLUserDataHolder> data = new ZLUserDataHolder;
	LitResBookshelfItemLoaderScope *scope = new LitResBookshelfItemLoaderScope;
	scope->children = &children;
	scope->listener = listener;
	data->addUserData("scope", scope);
	mgr.isAuthorised(ZLExecutionData::createListener(data, this, &LitResBookshelfItem::onAuthorised));
	return std::string();
}

void LitResBookshelfItem::onAuthorised(ZLUserDataHolder &data, const std::string &error) {
	LitResBookshelfItemLoaderScope &scope = static_cast<LitResBookshelfItemLoaderScope&>(*data.getUserData("scope"));
	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*Link.authenticationManager());
	if (!error.empty()) {
		scope.listener->finished(error);
		return;
	}
	if (myForceReload) {
		ZLUserDataHolder *dataCopy = new ZLUserDataHolder(data);
		mgr.reloadPurchasedBooks(ZLExecutionData::createListener(dataCopy, this, &LitResBookshelfItem::onReloaded));
		return;
	}
	onReloaded(data, error);
}

void LitResBookshelfItem::onReloaded(ZLUserDataHolder &data, const std::string &error) {
	LitResBookshelfItemLoaderScope &scope = static_cast<LitResBookshelfItemLoaderScope&>(*data.getUserData("scope"));
	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*Link.authenticationManager());
	myForceReload = true;
	mgr.collectPurchasedBooks(*scope.children);
	std::sort(scope.children->begin(), scope.children->end(), NetworkBookItemComparator());
	scope.listener->finished(error);
}
