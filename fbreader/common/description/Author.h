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

#ifndef __AUTHOR_H__
#define __AUTHOR_H__

#include <string>
#include <vector>

class Author {

protected:
	Author() {}

public:
	virtual ~Author() {}
	virtual Author *createCopy() const = 0;
	
	virtual const std::string &displayName() const = 0;
	virtual const std::string &sortKey() const = 0;
	virtual bool isSingle() const = 0;
};

class DummyAuthor : public Author {

public:
	DummyAuthor() : myDisplayName("Unknown Author"), mySortKey("UnknownAuthor") {}
	Author *createCopy() const { return new DummyAuthor(); }
	const std::string &displayName() const { return myDisplayName; }
	const std::string &sortKey() const { return mySortKey; }
	bool isSingle() const { return true; }

private:
	std::string myDisplayName;
	std::string mySortKey;
};

class SingleAuthorWith3Names : public Author {

public:
	SingleAuthorWith3Names(const std::string &firstName, const std::string &middleName, const std::string &lastName) : myFirstName(firstName), myMiddleName(middleName), myLastName(lastName) {}
	Author *createCopy() const;
	const std::string &displayName() const;
	const std::string &sortKey() const;
	bool isSingle() const { return true; }

private:
	std::string myFirstName;
	std::string myMiddleName;
	std::string myLastName;

	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class MultiAuthor : public Author {

public:
	MultiAuthor() {}
	~MultiAuthor();
	Author *createCopy() const;
	void addAuthor(Author *author);
	const std::string &displayName() const;
	const std::string &sortKey() const;
	bool isSingle() const { return false; }

private:
	std::vector<Author*> myAuthors;
	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class AuthorComparator {

public:
	bool operator() (const Author *a1, const Author *a2) { return a1->sortKey() < a2->sortKey(); }
};

#endif /* __AUTHOR_H__ */
