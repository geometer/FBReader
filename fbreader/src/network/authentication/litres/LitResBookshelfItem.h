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

#ifndef __LITRESBOOKSHELFITEM_H__
#define __LITRESBOOKSHELFITEM_H__

#include "../../NetworkItems.h"

class NetworkLink;

class LitResBookshelfItemLoader : public ZLExecutionData::Listener {
public:
	LitResBookshelfItemLoader(const NetworkLink &link, NetworkItem::List &children, bool forceReload, shared_ptr<ZLExecutionData::Listener> listener);
	~LitResBookshelfItemLoader();
	
	virtual void showPercent(int ready, int full);
	virtual void finished(const std::string &error = std::string());
	
private:
	enum State { Authorization, Loading };
	void die();
	const NetworkLink &myLink;
	NetworkItem::List &myChildren;
	bool myForceReload;
	State myState;
	shared_ptr<ZLExecutionData::Listener> myListener;
	shared_ptr<ZLExecutionData::Listener> myHolder;
};

class LitResBookshelfItem : public NetworkCatalogItem {

public:
	LitResBookshelfItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility = Always
	);

private:
	void onDisplayItem();
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLExecutionData::Listener> listener);

private:
	bool myForceReload;
};

#endif /* __LITRESBOOKSHELFITEM_H__ */
