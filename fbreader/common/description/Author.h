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

#ifndef __AUTHOR_H__
#define __AUTHOR_H__

#include <string>
#include <vector>

#include <abstract/shared_ptr.h>

class Author;
typedef shared_ptr<Author> AuthorPtr;

class Author {

protected:
	Author() MODEL_SECTION;

public:
	virtual ~Author() MODEL_SECTION;
	
	virtual const std::string &displayName() const MODEL_SECTION = 0;
	virtual const std::string &sortKey() const MODEL_SECTION = 0;
	virtual bool isSingle() const MODEL_SECTION = 0;

private:
	Author(const Author&);
	const Author &operator= (const Author&);
};

class SingleAuthor : public Author {

public:
	static AuthorPtr create(const std::string &displayName, const std::string &sortKey);
	static AuthorPtr create();
				
private:
	SingleAuthor(const std::string &displayName, const std::string &sortKey) MODEL_SECTION;

public:
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::string myDisplayName;
	std::string mySortKey;
};

class MultiAuthor : public Author {

public:
	static AuthorPtr create(AuthorPtr author);
				
private:
	MultiAuthor(AuthorPtr author) MODEL_SECTION;

public:
	void addAuthor(AuthorPtr author)  MODEL_SECTION;
	const std::string &displayName() const MODEL_SECTION;
	const std::string &sortKey() const MODEL_SECTION;
	bool isSingle() const MODEL_SECTION;

private:
	std::vector<AuthorPtr > myAuthors;
	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class AuthorComparator {

public:
	AuthorComparator() MODEL_SECTION;
	~AuthorComparator() MODEL_SECTION;
	bool operator() (const AuthorPtr a1, const AuthorPtr a2) MODEL_SECTION;
};

inline Author::Author() {}
inline Author::~Author() {}

inline AuthorPtr SingleAuthor::create(const std::string &displayName, const std::string &sortKey) { return new SingleAuthor(displayName, sortKey); }
inline AuthorPtr SingleAuthor::create() { return create("Unknown Author", "___"); }
inline SingleAuthor::SingleAuthor(const std::string &displayName, const std::string &sortKey) : myDisplayName(displayName), mySortKey(sortKey) {}
inline const std::string &SingleAuthor::displayName() const { return myDisplayName; }
inline const std::string &SingleAuthor::sortKey() const { return mySortKey; }
inline bool SingleAuthor::isSingle() const { return true; }

inline AuthorPtr MultiAuthor::create(AuthorPtr author) { return new MultiAuthor(author); }
inline MultiAuthor::MultiAuthor(AuthorPtr author) { addAuthor(author); }
inline bool MultiAuthor::isSingle() const { return false; }

inline AuthorComparator::AuthorComparator() {}
inline AuthorComparator::~AuthorComparator() {}
inline bool AuthorComparator::operator() (const AuthorPtr a1, const AuthorPtr a2) { return a1->sortKey() < a2->sortKey(); }

#endif /* __AUTHOR_H__ */
