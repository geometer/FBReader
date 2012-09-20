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

#ifndef __LITRESAUTHORSBYLETTERITEM_H__
#define __LITRESAUTHORSBYLETTERITEM_H__

#include <map>

#include "../../NetworkItems.h"
#include "LitResAuthorsItem.h"

class LitResAuthorsByLetterItem : public LitResAuthorsItem {

public:
	LitResAuthorsByLetterItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility,
		int flags = FLAGS_DEFAULT
	);

private:
	std::string loadChildren(NetworkItem::List &children);
	void addSubCatalog(std::string start, std::string end, unsigned int sum, NetworkItem::List &children, const LitResAuthorsParser::AuthorsList &authors);

private:
	static std::map<std::string, int> getLetterCounter(const LitResAuthorsParser::AuthorsList &authors);

private:
//	class AuthorsComparator {

//	public:
//		bool operator () (const LitResAuthorsParser::LitresAuthorData &author1,
//						  const LitResAuthorsParser::LitresAuthorData &author2) const;
//	};
};

//inline bool LitResAuthorsByLetterItem::AuthorsComparator::operator ()(const LitResAuthorsParser::LitresAuthorData &author1,
//															   const LitResAuthorsParser::LitresAuthorData &author2) const {
//	return author1.LastName.compare(author2.LastName) < 0;
//}

#endif /* __LITRESAUTHORSBYLETTERITEM_H__ */
