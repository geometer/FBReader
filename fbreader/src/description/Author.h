/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <shared_ptr.h>

class Author;
typedef shared_ptr<Author> AuthorPtr;

class Author {

protected:
	Author();

public:
	virtual ~Author();
	
	virtual const std::string &displayName() const = 0;
	virtual const std::string &sortKey() const = 0;
	virtual bool isSingle() const = 0;

private:
	Author(const Author&);
	const Author &operator= (const Author&);
};

class SingleAuthor : public Author {

public:
	static AuthorPtr create(const std::string &displayName, const std::string &sortKey);
	static AuthorPtr create();
				
private:
	SingleAuthor(const std::string &displayName, const std::string &sortKey);

public:
	const std::string &displayName() const;
	const std::string &sortKey() const;
	bool isSingle() const;

private:
	std::string myDisplayName;
	std::string mySortKey;
};

class MultiAuthor : public Author {

public:
	static AuthorPtr create(AuthorPtr author);
				
private:
	MultiAuthor(AuthorPtr author);

public:
	void addAuthor(AuthorPtr author) ;
	const std::string &displayName() const;
	const std::string &sortKey() const;
	bool isSingle() const;

private:
	std::vector<AuthorPtr > myAuthors;
	mutable std::string myDisplayName;
	mutable std::string mySortKey;
};

class AuthorComparator {

public:
	AuthorComparator();
	~AuthorComparator();
	bool operator() (const AuthorPtr a1, const AuthorPtr a2);
};

inline Author::Author() {}
inline Author::~Author() {}

inline AuthorPtr SingleAuthor::create(const std::string &displayName, const std::string &sortKey) { return new SingleAuthor(displayName, sortKey); }
inline AuthorPtr SingleAuthor::create() { return create("Unknown Author", "___"); }
inline SingleAuthor::SingleAuthor(const std::string &displayName, const std::string &sortKey) : myDisplayName(displayName), mySortKey(sortKey) {}

inline AuthorPtr MultiAuthor::create(AuthorPtr author) { return new MultiAuthor(author); }
inline MultiAuthor::MultiAuthor(AuthorPtr author) { addAuthor(author); }

inline AuthorComparator::AuthorComparator() {}
inline AuthorComparator::~AuthorComparator() {}
inline bool AuthorComparator::operator() (const AuthorPtr a1, const AuthorPtr a2) {
	return
		(a1->sortKey() < a2->sortKey()) ||
		((a1->sortKey() == a2->sortKey()) && (a1->displayName() < a2->displayName()));
}

#endif /* __AUTHOR_H__ */
