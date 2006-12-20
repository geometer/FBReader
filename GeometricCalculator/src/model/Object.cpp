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
