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

void Scene::setDescription(const std::string &description) {
	myDescription = description;
}

const std::string &Scene::description() const {
	return myDescription;
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

static unsigned int parsePointName(const std::string &name) {
	if ((name.length() < 2) || (name[0] != 'P')) {
		return 0;
	}

	unsigned int num = 0;

	for (size_t i = 1; i < name.length(); i++) {
		if ((name[i] < '0') || (name[i] > '9')) {
			return 0;
		}
		num = num * 10 + name[i] - '0';
	}

	return num;
}

unsigned int Scene::maxUsedPointNumber() {
	unsigned int maxUsedNumber = 0;
	for (std::list<ObjectPtr>::iterator it = myObjects.begin(); it != myObjects.end(); it++) {
		PointPtr ptr = *it;
		if (!ptr.isNull()) {
			unsigned int number = parsePointName(ptr->name());
			if (number > maxUsedNumber) {
				maxUsedNumber = number;
			}
		}
	}
	return maxUsedNumber;
}

void Scene::add(const ObjectPtr object) {
	PointPtr point = object;
	if (!point.isNull() && point->name().empty()) {
		std::string name = "P";
		ZLStringUtil::appendNumber(name, maxUsedPointNumber() + 1);
		point->setName(name);
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
