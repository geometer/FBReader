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

#ifndef __POINTPTR_H__
#define __POINTPTR_H__

#include "Object.h"

class Point;
class PointPtr;
class PointWeakPtr;

class PointPtr : public ObjectPtr {
	public:
		PointPtr();
		PointPtr(const PointPtr &ptr);
		PointPtr(const PointWeakPtr &ptr);
		PointPtr(const ObjectPtr &ptr);
		PointPtr(Point *point);
		const PointPtr &operator = (const PointPtr &ptr);
		const PointPtr &operator = (const PointWeakPtr &ptr);
		const PointPtr &operator = (const ObjectPtr &ptr);
		const PointPtr &operator = (Point *point);

		Point* operator -> () const;
		Point& operator * () const;
};

class PointWeakPtr : public ObjectWeakPtr {
	public:
		PointWeakPtr();
		PointWeakPtr(const PointPtr &ptr);
		PointWeakPtr(const PointWeakPtr &ptr);
		const PointWeakPtr &operator = (const PointPtr &ptr);
		const PointWeakPtr &operator = (const PointWeakPtr &ptr);

		Point* operator -> () const;
		Point& operator * () const;
};

#endif /* __POINTPTR_H__ */
