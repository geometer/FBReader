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

std::string LitResBookshelfItem::loadChildren(NetworkItem::List &children) {
	LitResAuthenticationManager &mgr =
		(LitResAuthenticationManager&)*Link.authenticationManager();
	if (mgr.isAuthorised().Status == B3_FALSE) {
		return NetworkErrors::errorMessage(NetworkErrors::ERROR_AUTHENTICATION_FAILED);
	}
	std::string error;
	if (myForceReload) {
		error = mgr.reloadPurchasedBooks();
	}
	myForceReload = true;
	mgr.collectPurchasedBooks(children);
	std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	return error;
}
