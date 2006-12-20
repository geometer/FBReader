#include "AddCircleMode.h"
#include "../model/Circle.h"

AddCircleMode::AddCircleMode(DiagramView &view) : AddObjectMode(view) {
}

ObjectPtr AddCircleMode::createObject(PointPtr point0, PointPtr point1) const {
	return CircleByCenterAndPoint::create(point0, point1);
}
