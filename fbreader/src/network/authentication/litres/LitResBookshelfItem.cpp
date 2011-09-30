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

LitResBookshelfItemLoader::LitResBookshelfItemLoader(const NetworkLink &link, NetworkItem::List &children,
                                                     bool forceReload, shared_ptr<ZLExecutionData::Listener> listener)
    : myLink(link), myChildren(children), myForceReload(forceReload), myState(Authorization), myListener(listener), myHolder(this) {
	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*myLink.authenticationManager());
	NetworkAuthenticationManager::AuthenticationStatus status =  mgr.isAuthorised(true, myHolder);
	if (status.Status == B3_UNDEFINED)
		return;
	if (status.Status == B3_FALSE)
		finished(NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED));
	else
		finished(std::string());
}

LitResBookshelfItemLoader::~LitResBookshelfItemLoader() {
}

void LitResBookshelfItemLoader::showPercent(int ready, int full) {
	myListener->showPercent(ready, full);
}

void LitResBookshelfItemLoader::finished(const std::string &error) {
	if (myState == Authorization) {
		if (!error.empty()) {
			myListener->finished(error);
		}
	}
}

void LitResBookshelfItemLoader::die() {
	ZLTimeManager::deleteLater(myHolder);
	myHolder.reset();
}

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

std::string LitResBookshelfItem::loadChildren(NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener) {
	new LitResBookshelfItemLoader(Link, children, myForceReload, listener);
	myForceReload = true;
	return std::string();

//	LitResAuthenticationManager &mgr = static_cast<LitResAuthenticationManager&>(*Link.authenticationManager());
//	if (mgr.isAuthorised().Status == B3_FALSE) {
//		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
//	}
//	std::string error;
//	if (myForceReload) {
//		error = mgr.reloadPurchasedBooks();
//	}
//	myForceReload = true;
//	mgr.collectPurchasedBooks(children);
//	std::sort(children.begin(), children.end(), NetworkBookItemComparator());
//	return error;
}
