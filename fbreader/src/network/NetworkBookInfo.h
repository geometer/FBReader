/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKBOOKINFO_H__
#define __NETWORKBOOKINFO_H__

#include <string>
#include <map>
#include <vector>

#include <shared_ptr.h>

struct NetworkBookInfo {
	enum URLType {
		BOOK_EPUB,
		BOOK_MOBIPOCKET,
		BOOK_PDF,
		LINK_HTTP,
	};

	const std::string Id;

	std::string Title;
	struct AuthorData {
		std::string DisplayName;
		std::string SortKey;

		bool operator < (const AuthorData &data) const;
	};
	AuthorData Author;
	std::string Identifier;
	std::string Language;
	std::vector<std::string> Tags;
	std::string Cover;
	std::string Date;
	std::string Series;
	std::string Annotation;
	std::map<URLType,std::string> URLByType;

	NetworkBookInfo(const std::string &id);
	std::string fileName(URLType format) const;

	struct Comparator {
		bool operator () (const shared_ptr<NetworkBookInfo> &first, const shared_ptr<NetworkBookInfo> &second);
	};
};

typedef std::vector<shared_ptr<NetworkBookInfo> > NetworkBookList;

#endif /* __NETWORKBOOKINFO_H__ */
