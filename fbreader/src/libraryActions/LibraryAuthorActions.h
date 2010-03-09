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

#ifndef __LIBRARYAUTHORACTIONS_H__
#define __LIBRARYAUTHORACTIONS_H__

#include <shared_ptr.h>

#include <ZLRunnable.h>

class Author;

class AuthorEditInfoAction : public ZLRunnableWithKey {

public:
	AuthorEditInfoAction(shared_ptr<Author> author);
	~AuthorEditInfoAction();
	void run();
	ZLResourceKey key() const;

private:
	const shared_ptr<Author> myAuthor;
};

#endif /* __LIBRARYAUTHORACTIONS_H__ */
