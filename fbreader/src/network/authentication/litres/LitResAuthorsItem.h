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

#ifndef __LITRESAUTHORSITEM_H__
#define __LITRESAUTHORSITEM_H__

#include "../../NetworkItems.h"

#include "LitResAuthorsParser.h"

class LitResAuthorsItem : public NetworkCatalogItem {

public:
	LitResAuthorsItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility,
		int flags = FLAGS_DEFAULT
	);

protected:
	void fillChildrenWithAuthors(NetworkItem::List &children, const LitResAuthorsParser::AuthorsList &authors);
	std::string loadChildren(NetworkItem::List &children);

	static std::string getSubtitle(const LitResAuthorsParser::LitresAuthorData &author);
};

#endif /* __LITRESAUTHORSITEM_H__ */
