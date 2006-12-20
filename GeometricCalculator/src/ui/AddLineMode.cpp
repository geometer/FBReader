#include "AddLineMode.h"
#include "../model/Line.h"

AddLineMode::AddLineMode(DiagramView &view, LineKind kind) : AddObjectMode(view), myKind(kind) {
}

ObjectPtr AddLineMode::createObject(PointPtr point0, PointPtr point1) const {
	return LineByTwoPoints::create(point0, point1, myKind);
}
