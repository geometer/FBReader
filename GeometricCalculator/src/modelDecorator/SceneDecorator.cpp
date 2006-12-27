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
