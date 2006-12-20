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
