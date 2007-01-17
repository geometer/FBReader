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

#include "Circle.h"
#include "CirclePtr.h"

CirclePtr::CirclePtr() : ObjectPtr(NULL) {
}

CirclePtr::CirclePtr(const CirclePtr &ptr) : ObjectPtr(ptr) {
}

CirclePtr::CirclePtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

CirclePtr::CirclePtr(Circle *circle) : ObjectPtr(circle) {
}

const CirclePtr &CirclePtr::operator = (const CirclePtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const CirclePtr &CirclePtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Object::CIRCLE) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (NULL);
	}
	return *this;
}

const CirclePtr &CirclePtr::operator = (Circle *circle) {
	ObjectPtr::operator = (circle);
	return *this;
}

Circle* CirclePtr::operator -> () const {
	return (Circle*)ObjectPtr::operator -> ();
}

Circle& CirclePtr::operator * () const {
	return (Circle&)ObjectPtr::operator * ();
}
