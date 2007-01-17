/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __LINEPTR_H__
#define __LINEPTR_H__

#include "Object.h"

class Line;
class LinePtr;
class LineWeakPtr;

class LinePtr : public ObjectPtr {
	public:
		LinePtr();
		LinePtr(const LinePtr &ptr);
		LinePtr(const LineWeakPtr &ptr);
		LinePtr(const ObjectPtr &ptr);
		LinePtr(Line *line);
		const LinePtr &operator = (const LinePtr &ptr);
		const LinePtr &operator = (const LineWeakPtr &ptr);
		const LinePtr &operator = (const ObjectPtr &ptr);
		const LinePtr &operator = (Line *line);

		Line* operator -> () const;
		Line& operator * () const;
};

class LineWeakPtr : public ObjectWeakPtr {
	public:
		LineWeakPtr();
		LineWeakPtr(const LinePtr &ptr);
		LineWeakPtr(const LineWeakPtr &ptr);
		const LineWeakPtr &operator = (const LinePtr &ptr);
		const LineWeakPtr &operator = (const LineWeakPtr &ptr);

		Line* operator -> () const;
		Line& operator * () const;
};

#endif /* __LINEPTR_H__ */
