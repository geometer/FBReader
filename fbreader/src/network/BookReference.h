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

#ifndef __BOOKREFERENCE_H__
#define __BOOKREFERENCE_H__

#include <string>

class BookReference {

public:
	enum Type {
		DOWNLOAD_FULL,
		DOWNLOAD_FULL_CONDITIONAL,
		DOWNLOAD_DEMO,
		DOWNLOAD_FULL_OR_DEMO,
		BUY,
		BUY_IN_BROWSER,
		UNKNOWN
	};

	enum Format {
		NONE = 0,
		MOBIPOCKET = 1,
		FB2_ZIP = 2,
		EPUB = 3,
	};

public:
	BookReference(const std::string &url, Format format, Type type);

public:
	const std::string URL;
	const Format BookFormat;
	const Type ReferenceType;

public:
	virtual const std::string &cleanURL() const;

	std::string localCopyFileName() const;

private:
	BookReference(const BookReference&);
	const BookReference &operator = (const BookReference&);
};

class BuyBookReference : public BookReference {

public:
	BuyBookReference(const std::string &url, Format format, Type type, const std::string &price);

public:
	static std::string price(const std::string &price, const std::string &currency);

public:
	const std::string Price;
};

class DecoratedBookReference : public BookReference {

public:
	DecoratedBookReference(const BookReference &base, const std::string &url);

private:
	const std::string &cleanURL() const;

private:
	const std::string myCleanURL;
};

#endif /* __BOOKREFERENCE_H__ */
