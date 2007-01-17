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

ObjectEnumerator::ObjectEnumerator() {
}

int ObjectEnumerator::id(ObjectPtr object) const {
	std::map<ObjectPtr,int>::const_iterator it = myObjectToIdMap.find(object);
	return (it != myObjectToIdMap.end()) ? (*it).second : -1;
}

ObjectPtr ObjectEnumerator::object(int id) const {
	std::map<int,ObjectPtr>::const_iterator it = myIdToObjectMap.find(id);
	return (it != myIdToObjectMap.end()) ? (*it).second : 0;
}

void ObjectEnumerator::addObject(ObjectPtr object, int id) {
	if (id == -1) {
		id = myObjectToIdMap.size();
	}
	myObjectToIdMap[object] = id;
	myIdToObjectMap[id] = object;
}

void ObjectEnumerator::clear() {
	myObjectToIdMap.clear();
	myIdToObjectMap.clear();
}
