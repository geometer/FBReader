/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __AUTHOR_H__
#define __AUTHOR_H__

#include <string>
#include <vector>

class Author {

protected:
	Author() MODEL_SECTION;

public:
	virtual ~Author() MODEL_SECTION;
	virtual Author *createCopy() const MODEL_SECTION = 0;
	
	virtual const std::string &displayName() const MODEL_SECTION = 0;
	virtual const std::string &sortKey() const MODEL_SECTION = 0;
	virtual bool isSingle() const MODEL_SECTION = 0;
};

class DummyAuthor : public Author {

public:
	DummyAuthor() MODEL_SECTION;
	~DummyAuthor() MODEL_SECTION;
	Author *createCopy() const MODEL_SECTION;
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::string myDisplayName;
	std::string mySortKey;
};

class StoredAuthor : public Author {

public:
	StoredAuthor(const std::string &displayName, const std::string &sortKey) MODEL_SECTION;
	~StoredAuthor()  MODEL_SECTION;
	Author *createCopy() const MODEL_SECTION;
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::string myDisplayName;
	std::string mySortKey;
};

class SingleAuthorWith3Names : public Author {

public:
	SingleAuthorWith3Names(const std::string &firstName, const std::string &middleName, const std::string &lastName) MODEL_SECTION;
	~SingleAuthorWith3Names() MODEL_SECTION;
	Author *createCopy() const MODEL_SECTION;
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::string myFirstName;
	std::string myMiddleName;
	std::string myLastName;

	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class MultiAuthor : public Author {

public:
	MultiAuthor() MODEL_SECTION;
	~MultiAuthor()  MODEL_SECTION;
	Author *createCopy() const MODEL_SECTION;
	void addAuthor(Author *author)  MODEL_SECTION;
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::vector<Author*> myAuthors;
	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class AuthorComparator {

public:
	AuthorComparator() MODEL_SECTION;
	~AuthorComparator() MODEL_SECTION;
	bool operator() (const Author *a1, const Author *a2) MODEL_SECTION;
};

inline Author::Author() {}
inline Author::~Author() {}

inline DummyAuthor::DummyAuthor() : myDisplayName("Unknown Author"), mySortKey("UnknownAuthor") {}
inline DummyAuthor::~DummyAuthor() {}
inline Author *DummyAuthor::createCopy() const { return new DummyAuthor(); }
inline const std::string &DummyAuthor::displayName() const { return myDisplayName; }
inline const std::string &DummyAuthor::sortKey() const { return mySortKey; }
inline bool DummyAuthor::isSingle() const { return true; }

inline StoredAuthor::StoredAuthor(const std::string &displayName, const std::string &sortKey) : myDisplayName(displayName), mySortKey(sortKey) {}
inline StoredAuthor::~StoredAuthor() {}
inline Author *StoredAuthor::createCopy() const { return new StoredAuthor(myDisplayName, mySortKey); }
inline const std::string &StoredAuthor::displayName() const { return myDisplayName; }
inline const std::string &StoredAuthor::sortKey() const { return mySortKey; }
inline bool StoredAuthor::isSingle() const { return true; }

inline SingleAuthorWith3Names::SingleAuthorWith3Names(const std::string &firstName, const std::string &middleName, const std::string &lastName) : myFirstName(firstName), myMiddleName(middleName), myLastName(lastName) {}
inline SingleAuthorWith3Names::~SingleAuthorWith3Names() {}
inline bool SingleAuthorWith3Names::isSingle() const { return true; }

inline MultiAuthor::MultiAuthor() {}
inline bool MultiAuthor::isSingle() const { return false; }

inline AuthorComparator::AuthorComparator() {}
inline AuthorComparator::~AuthorComparator() {}
inline bool AuthorComparator::operator() (const Author *a1, const Author *a2) { return a1->sortKey() < a2->sortKey(); }

//inline bool operator < (const Author *a1, const Author *a2) { return a1->sortKey() < a2->sortKey(); }

#endif /* __AUTHOR_H__ */
