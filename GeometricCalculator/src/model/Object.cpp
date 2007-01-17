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

#include "Object.h"
#include "Point.h"

static const std::list<ObjectPtr> EMPTY_OBJECT_LIST;

double Object::distance(const Coordinates coords) const {
	return distance(coords.x, coords.y);
}

double Object::distance(PointPtr point) const {
	return distance(point->coordinates());
}

const std::list<ObjectPtr> &Object::dependsOn() const {
	return EMPTY_OBJECT_LIST;
}

bool Object::isReal() const {
	const std::list<ObjectPtr> &objects = dependsOn();
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if (!(*it)->isReal()) {
			return false;
		}
	}
	return true;
}
