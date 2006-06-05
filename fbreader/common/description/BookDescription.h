/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __BOOKDESCRIPTION_H__
#define __BOOKDESCRIPTION_H__

#include <string>

#include <abstract/shared_ptr.h>

#include <abstract/ZLOptions.h>

#include "Author.h"

class BookDescription;
typedef shared_ptr<BookDescription> BookDescriptionPtr;

struct BookInfo {
	BookInfo(const std::string &fileName);
	~BookInfo();

	bool isFull() const;
	void reset();

	ZLStringOption AuthorDisplayNameOption;
	ZLStringOption AuthorSortKeyOption;
	ZLStringOption TitleOption;
	ZLStringOption LanguageOption;
	ZLStringOption EncodingOption;

	ZLStringOption AuthorDisplayNameEncodedOption;
	ZLStringOption AuthorSortKeyEncodedOption;
	ZLStringOption TitleEncodedOption;
};

class BookDescription {

public:
	static BookDescriptionPtr create(const std::string &fileName);

private:
	BookDescription(const std::string &fileName);

public:
	const AuthorPtr author() const;
	const std::string &title() const;
	const std::string &fileName() const;
	const std::string &language() const;
	const std::string &encoding() const;

private:
	AuthorPtr myAuthor;
	std::string myTitle;
	std::string myFileName;
	std::string myLanguage;
	std::string myEncoding;

friend class WritableBookDescription;

private:
	// disable copying
	BookDescription(const BookDescription &description);
	const BookDescription &operator = (const BookDescription &description);
};

class WritableBookDescription {

public:
	WritableBookDescription(BookDescription &description);
	~WritableBookDescription();
	void addAuthor(const std::string &name);
	void addAuthor(const std::string &name, const std::string &sortKey);
	const AuthorPtr author() const;
	std::string &title();
	std::string &fileName();
	std::string &language();
	std::string &encoding();

private:
	BookDescription &myDescription;
};

inline BookInfo::~BookInfo() {}

inline const AuthorPtr BookDescription::author() const { return myAuthor; }
inline const std::string &BookDescription::title() const { return myTitle; }
inline const std::string &BookDescription::fileName() const { return myFileName; }
inline const std::string &BookDescription::language() const { return myLanguage; }
inline const std::string &BookDescription::encoding() const { return myEncoding; }

inline WritableBookDescription::WritableBookDescription(BookDescription &description) : myDescription(description) {}
inline WritableBookDescription::~WritableBookDescription() {}
inline const AuthorPtr WritableBookDescription::author() const { return myDescription.author(); }
inline std::string &WritableBookDescription::title() { return myDescription.myTitle; }
inline std::string &WritableBookDescription::fileName() { return myDescription.myFileName; }
inline std::string &WritableBookDescription::language() { return myDescription.myLanguage; }
inline std::string &WritableBookDescription::encoding() { return myDescription.myEncoding; }

#endif /* __BOOKDESCRIPTION_H__ */
