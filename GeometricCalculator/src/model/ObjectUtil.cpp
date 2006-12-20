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
