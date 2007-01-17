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

#include "ObjectUtil.h"
#include "Point.h"

#include <set>
#include <queue>

std::list<ObjectPtr> ObjectUtil::orderedClosure(const std::list<ObjectPtr> &objects) {
	std::list<ObjectPtr> closureList;

	std::set<ObjectPtr> allObjects;
	std::set<ObjectPtr> addedObjects;

	std::queue<ObjectPtr> objectsQueue;

	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		allObjects.insert(*it);
		objectsQueue.push(*it);
	}

	while (!objectsQueue.empty()) {
		bool dependencySaved = true;
		ObjectPtr object = objectsQueue.front();
		objectsQueue.pop();
		for (std::list<ObjectPtr>::const_iterator dep = object->dependsOn().begin(); dep != object->dependsOn().end(); dep++) {
			if (allObjects.find(*dep) == allObjects.end()) {
				allObjects.insert(*dep);
				objectsQueue.push(*dep);
				dependencySaved = false;
			} else if (addedObjects.find(*dep) == addedObjects.end()) {
				dependencySaved = false;
			}
		}
		if (dependencySaved) {
			closureList.push_back(object);
			addedObjects.insert(object);
		} else {
			objectsQueue.push(object);
		}
	}

	return closureList;
}
