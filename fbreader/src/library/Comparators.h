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

#ifndef __COMPARATORS_H__
#define __COMPARATORS_H__

#include <shared_ptr.h>

class Book;
class Author;
class Tag;

class BookComparator {

public:
	bool operator () (
		const shared_ptr<Book> book0,
		const shared_ptr<Book> book1
	);
};

class BookByFileNameComparator {

public:
	bool operator () (
		const shared_ptr<Book> book0,
		const shared_ptr<Book> book1
	);
};

class AuthorComparator {

public:
	bool operator () (
		const shared_ptr<Author> author0,
		const shared_ptr<Author> author1
	);
};

class TagComparator {

public:
	bool operator () (
		shared_ptr<Tag> tag0,
		shared_ptr<Tag> tag1
	);
};

#endif /* __COMPARATORS_H__ */
