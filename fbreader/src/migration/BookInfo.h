/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __BOOKINFO_H__
#define __BOOKINFO_H__

#include <string>

#include <ZLOptions.h>


struct BookInfo {
	BookInfo(const std::string &fileName);
	~BookInfo();

	bool isFull() const;
	void reset();

	ZLStringOption AuthorDisplayNameOption;
	ZLStringOption AuthorSortKeyOption;
	ZLStringOption TitleOption;
	ZLStringOption SeriesTitleOption;
	ZLIntegerRangeOption IndexInSeriesOption;
	ZLStringOption LanguageOption;
	ZLStringOption EncodingOption;
	ZLStringOption TagsOption;
	
	const BookInfo &operator = (const BookInfo &bi);
};

inline BookInfo::~BookInfo() {}

#endif /* __BOOKINFO_H__ */
