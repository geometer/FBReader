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

class Author;

class BookDescription {

public:
	static BookDescription *create(const std::string &fileName);
	BookDescription(const BookDescription &description);
	~BookDescription();

private:
	BookDescription(const std::string &fileName);

public:
	const Author *author() const { return myAuthor; }
	const std::string &title() const { return myTitle; }
	const std::string &fileName() const { return myFileName; }
	const std::string &language() const { return myLanguage; }
	const std::string &encoding() const { return myEncoding; }

private:
	Author *myAuthor;
	std::string myTitle;
	std::string myFileName;
	std::string myLanguage;
	std::string myEncoding;

friend class WritableBookDescription;
};

class WritableBookDescription {

public:
	WritableBookDescription(BookDescription &description) : myDescription(description) {}
	void addAuthor(const std::string &firstName, const std::string &middleName, const std::string &lastName);
	std::string &title() { return myDescription.myTitle; }
	std::string &fileName() { return myDescription.myFileName; }
	std::string &language() { return myDescription.myLanguage; }
	std::string &encoding() { return myDescription.myEncoding; }

private:
	BookDescription &myDescription;
};

#endif /* __BOOKDESCRIPTION_H__ */
