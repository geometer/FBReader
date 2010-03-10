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

#ifndef __LIBRARYBOOKACTIONS_H__
#define __LIBRARYBOOKACTIONS_H__

#include <shared_ptr.h>

#include <ZLRunnable.h>

class Book;

class BookReadAction : public ZLRunnableWithKey {

public:
	BookReadAction(shared_ptr<Book> book);
	void run();
	ZLResourceKey key() const;

private:
	const shared_ptr<Book> myBook;
};

class BookEditInfoAction : public ZLRunnableWithKey {

public:
	BookEditInfoAction(shared_ptr<Book> book);
	void run();
	ZLResourceKey key() const;

private:
	const shared_ptr<Book> myBook;
};

class BookRemoveAction : public ZLRunnableWithKey {

public:
	BookRemoveAction(shared_ptr<Book> book);

private:
	void run();
	ZLResourceKey key() const;
	bool makesSense() const;

	int removeBookDialog() const;

private:
	const shared_ptr<Book> myBook;
};

#endif /* __LIBRARYBOOKACTIONS_H__ */
