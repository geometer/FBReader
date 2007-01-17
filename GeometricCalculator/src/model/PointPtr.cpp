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

#include "Point.h"

PointPtr::PointPtr() {
}

PointPtr::PointPtr(const PointPtr &ptr) : ObjectPtr(ptr) {
}

PointPtr::PointPtr(const PointWeakPtr &ptr) : ObjectPtr(ptr) {
}

PointPtr::PointPtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

PointPtr::PointPtr(Point *point) : ObjectPtr(point) {
}

const PointPtr &PointPtr::operator = (const PointPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const PointPtr &PointPtr::operator = (const PointWeakPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const PointPtr &PointPtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Object::POINT) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (0);
	}
	return *this;
}

const PointPtr &PointPtr::operator = (Point *point) {
	ObjectPtr::operator = (point);
	return *this;
}

Point* PointPtr::operator -> () const {
	return (Point*)ObjectPtr::operator -> ();
}

Point& PointPtr::operator * () const {
	return (Point&)ObjectPtr::operator * ();
}
