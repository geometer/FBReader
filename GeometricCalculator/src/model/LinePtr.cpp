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

#include "Line.h"

LinePtr::LinePtr() : ObjectPtr(NULL) {
}

LinePtr::LinePtr(const LinePtr &ptr) : ObjectPtr(ptr) {
}

LinePtr::LinePtr(const LineWeakPtr &ptr) : ObjectPtr(ptr) {
}

LinePtr::LinePtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

LinePtr::LinePtr(Line *line) : ObjectPtr(line) {
}

const LinePtr &LinePtr::operator = (const LinePtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const LinePtr &LinePtr::operator = (const LineWeakPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const LinePtr &LinePtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Object::LINE) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (NULL);
	}
	return *this;
}

const LinePtr &LinePtr::operator = (Line *line) {
	ObjectPtr::operator = (line);
	return *this;
}

Line* LinePtr::operator -> () const {
	return (Line*)ObjectPtr::operator -> ();
}

Line& LinePtr::operator * () const {
	return (Line&)ObjectPtr::operator * ();
}
