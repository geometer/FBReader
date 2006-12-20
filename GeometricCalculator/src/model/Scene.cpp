#include <ZLStringUtil.h>

#include "Scene.h"
#include "Object.h"
#include "Point.h"

void Scene::setName(const std::string &name) {
	myName = name;
}

const std::string &Scene::name() const {
	return myName;
}

bool Scene::isEmpty() const {
	return myObjects.empty() && myValues.empty();
}

const std::list<ObjectPtr> &Scene::objects() const {
	return myObjects;
}

const std::list<ValuePtr> &Scene::values() const {
	return myValues;
}

bool Scene::isPersistent(const ObjectPtr object) const {
	return const_find(object) != myObjects.end();
}

static uint parsePointName(const std::string &name) {
	if ((name.length() < 2) || (name[0] != 'P')) {
		return 0;
	}

	uint num = 0;

	for (size_t i = 1; i < name.length(); i++) {
		if ((name[i] < '0') || (name[i] > '9')) {
			return 0;
		}
		num = num * 10 + name[i] - '0';
	}

	return num;
}

uint Scene::maxUsedPointNumber() {
	uint maxUsedNumber = 0;
	for (std::list<ObjectPtr>::iterator it = myObjects.begin(); it != myObjects.end(); it++) {
		PointPtr ptr = *it;
		if (!ptr.isNull()) {
			uint number = parsePointName(ptr->name());
			if (number > maxUsedNumber) {
				maxUsedNumber = number;
			}
		}
	}
	return maxUsedNumber;
}

void Scene::add(const ObjectPtr object) {
	if (object->rtti() == Point::RTTI) {
		PointPtr point = object;
		if (point->name().empty()) {
			std::string name = "P";
			ZLStringUtil::appendNumber(name, maxUsedPointNumber() + 1);
			point->setName(name);
		}
	}
	myObjects.push_back(object);
}

void Scene::remove(const ObjectPtr object) {
	myObjects.remove(object);
}

void Scene::add(const ValuePtr value) {
	myValues.push_back(value);
}

void Scene::remove(const ValuePtr value) {
	myValues.remove(value);
}

std::list<ObjectPtr>::iterator Scene::find(const ObjectPtr object) {
	for (std::list<ObjectPtr>::iterator it = myObjects.begin(); it != myObjects.end(); it++) {
		if (*it == object) {
			return it;
		}
	}
	return myObjects.end();
}

std::list<ObjectPtr>::const_iterator Scene::const_find(const ObjectPtr object) const {
	for (std::list<ObjectPtr>::const_iterator it = myObjects.begin(); it != myObjects.end(); it++) {
		if (*it == object) {
			return it;
		}
	}
	return myObjects.end();
}
