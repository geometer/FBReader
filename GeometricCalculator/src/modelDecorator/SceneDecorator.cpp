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

#include "Decorator.h"

SceneDecorator::SceneDecorator(shared_ptr<Scene> &scene) {
	for (std::list<ObjectPtr>::const_iterator it = scene->objects().begin(); it != scene->objects().end(); it++) {
		add(*it);
	}
}

shared_ptr<Scene> SceneDecorator::restore() {
	shared_ptr<Scene> scene = new Scene();
	for (std::list<ObjectPtr>::const_iterator it = myObjects.begin(); it != myObjects.end(); it++) {
		if ((*it)->rtti() == Object::POINT) {
			scene->add(((PointDecorator&)**it).restore());
		} else {
			scene->add(*it);
		}
	}
	
	return scene;
}

void SceneDecorator::add(ObjectPtr object) {
	PointPtr point = object;
	if (!point.isNull()) {
		point = new PointDecorator(point);
		Scene::add(point);
		return;
	}

	Scene::add(object);
}

std::list<ObjectPtr>::iterator SceneDecorator::find(ObjectPtr object) {
	if (object.isNull()) {
		return myObjects.end();
	}
	if (object->rtti() == Object::POINT) {
		for (std::list<ObjectPtr>::iterator it = myObjects.begin(); it != myObjects.end(); it++) {
			if ((*it)->rtti() == Object::POINT) {
				if (((PointDecorator&)**it).stored() == object) {
					return it;
				}
			}
		}
		return myObjects.end();
	}
	return Scene::find(object);
}

std::list<ObjectPtr>::const_iterator SceneDecorator::const_find(ObjectPtr object) const {
	if (object.isNull()) {
		return myObjects.end();
	}
	if (object->rtti() == Object::POINT) {
		for (std::list<ObjectPtr>::const_iterator it = myObjects.begin(); it != myObjects.end(); it++) {
			if ((*it)->rtti() == Object::POINT) {
				if (((PointDecorator&)**it).stored() == object) {
					return it;
				}
			}
		}
		return myObjects.end();
	}
	return Scene::const_find(object);
}

void SceneDecorator::remove(ObjectPtr object) {
	std::list<ObjectPtr>::iterator it = find(object);
	if (it != myObjects.end()) {
		myObjects.erase(it);
	}
}
