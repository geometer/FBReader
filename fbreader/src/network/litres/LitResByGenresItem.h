/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESBYGENRESITEM_H__
#define __LITRESBYGENRESITEM_H__

#include "../NetworkItems.h"

#include "LitResGenre.h"

class LitResByGenresItem : public NetworkCatalogItem {

public:
	LitResByGenresItem(
		const std::vector<shared_ptr<LitResGenre> > &genreTree,
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const UrlInfoCollection &urlByType,
		AccessibilityType accessibility,
		int flags
	);

private:
	std::string loadChildren(NetworkItem::List &children, shared_ptr<ZLNetworkRequest::Listener> listener);

private:
	const std::vector<shared_ptr<LitResGenre> > &myGenreTree;
};

#endif /* __LITRESBYGENRESITEM_H__ */
