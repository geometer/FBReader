/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __BOOKDESCRIPTION_H__
#define __BOOKDESCRIPTION_H__

#include <string>

#include <abstract/shared_ptr.h>

#include <abstract/ZLOptions.h>

class Author;
class BookDescription;
typedef shared_ptr<BookDescription> BookDescriptionPtr;

struct BookInfo {
	BookInfo(const std::string &fileName) MODEL_SECTION;
	~BookInfo() MODEL_SECTION;

	bool isFull() const MODEL_SECTION;
	void reset() const MODEL_SECTION;

	ZLStringOption AuthorDisplayNameOption;
	ZLStringOption AuthorSortKeyOption;
	ZLStringOption TitleOption;
	ZLStringOption LanguageOption;
	ZLStringOption EncodingOption;
};

class BookDescription {

public:
	static BookDescriptionPtr create(const std::string &fileName) MODEL_SECTION;
	~BookDescription() MODEL_SECTION;

private:
	BookDescription(const std::string &fileName) MODEL_SECTION;

public:
	const Author *author() const MODEL_SECTION;
	const std::string &title() const MODEL_SECTION;
	const std::string &fileName() const MODEL_SECTION;
	const std::string &language() const MODEL_SECTION;
	const std::string &encoding() const MODEL_SECTION;

private:
	Author *myAuthor;
	std::string myTitle;
	std::string myFileName;
	std::string myLanguage;
	std::string myEncoding;

friend class WritableBookDescription;

private:
	// disable copying
	BookDescription(const BookDescription &description) MODEL_SECTION;
	const BookDescription &operator = (const BookDescription &description) MODEL_SECTION;
};

class WritableBookDescription {

public:
	WritableBookDescription(BookDescription &description) MODEL_SECTION;
	~WritableBookDescription() MODEL_SECTION;
	void addAuthor(const std::string &firstName, const std::string &middleName, const std::string &lastName) MODEL_SECTION;
	std::string &title() MODEL_SECTION;
	std::string &fileName() MODEL_SECTION;
	std::string &language() MODEL_SECTION;
	std::string &encoding() MODEL_SECTION;

private:
	BookDescription &myDescription;
};

inline BookInfo::~BookInfo() {}

inline const Author *BookDescription::author() const { return myAuthor; }
inline const std::string &BookDescription::title() const { return myTitle; }
inline const std::string &BookDescription::fileName() const { return myFileName; }
inline const std::string &BookDescription::language() const { return myLanguage; }
inline const std::string &BookDescription::encoding() const { return myEncoding; }

inline WritableBookDescription::WritableBookDescription(BookDescription &description) : myDescription(description) {}
inline WritableBookDescription::~WritableBookDescription() {}
inline std::string &WritableBookDescription::title() { return myDescription.myTitle; }
inline std::string &WritableBookDescription::fileName() { return myDescription.myFileName; }
inline std::string &WritableBookDescription::language() { return myDescription.myLanguage; }
inline std::string &WritableBookDescription::encoding() { return myDescription.myEncoding; }

#endif /* __BOOKDESCRIPTION_H__ */
