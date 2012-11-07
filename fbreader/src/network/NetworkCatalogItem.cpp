/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include "authentication/NetworkAuthenticationManager.h"
#include "NetworkLink.h"

#include "NetworkItems.h"

const ZLTypeId NetworkCatalogItem::TYPE_ID(NetworkItem::TYPE_ID);

NetworkCatalogItem::NetworkCatalogItem(
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility,
	int flags
) :
	NetworkItem(link, title, summary, urlByType),
	myAccessibility(accessibility),
	myFlags(flags)
	{
}

const ZLTypeId &NetworkCatalogItem::typeId() const {
	return TYPE_ID;
}

void NetworkCatalogItem::onDisplayItem() {
}

bool NetworkCatalogItem::supportsResumeLoading() {
	return false;
}

std::string NetworkCatalogItem::resumeLoading(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener) {
	listener->finished();
	return std::string();
}

std::string NetworkCatalogItem::getCatalogUrl() {
	return URLByType[URL_CATALOG];
}

int NetworkCatalogItem::getFlags() const {
	return myFlags;
}

NetworkCatalogItem::AccessibilityType NetworkCatalogItem::getAccessibility() const {
	return myAccessibility;
}

ZLBoolean3 NetworkCatalogItem::getVisibility() const {
	shared_ptr<NetworkAuthenticationManager> mgr = Link.authenticationManager();
	switch (myAccessibility) {
		case ALWAYS:
			return B3_TRUE;
		case SIGNED_IN:
			if (mgr.isNull()) {
				return B3_FALSE;
			}
			return mgr->isAuthorised().Status;
	}
	return B3_FALSE;
}
