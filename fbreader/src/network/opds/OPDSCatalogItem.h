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

#ifndef __OPDSCATALOGITEM_H__
#define __OPDSCATALOGITEM_H__

#include "../NetworkItems.h"
#include "../NetworkOperationData.h"

class OPDSLink;

class OPDSCatalogItemLoader : public ZLExecutionData::Listener {
public:
	OPDSCatalogItemLoader(const OPDSLink &link, const std::string &url, NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener);
	~OPDSCatalogItemLoader();
	
	virtual void showPercent(int ready, int full);
	virtual void finished(const std::string &error = std::string());
	
private:
	const OPDSLink &myLink;
	NetworkItem::List &myChildren;
	NetworkOperationData myData;
	shared_ptr<ZLExecutionData::Listener> myListener;
	shared_ptr<ZLExecutionData::Listener> myHolder;
};

class OPDSCatalogItem : public NetworkCatalogItem {

public:
	OPDSCatalogItem(
		const OPDSLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility = Always,
		CatalogType catalogType = OTHER
	);

private:
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener);
};

#endif /* __OPDSCATALOGITEM_H__ */
