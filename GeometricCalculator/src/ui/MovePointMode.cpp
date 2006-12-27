#include "MovePointMode.h"
#include "../document/Document.h"
#include "../model/Point.h"

MovePointMode::MovePointMode(DiagramView &view) : EditMode(view) {
}

void MovePointMode::release() {
	myPoint.reset();
}

bool MovePointMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if ((*it)->rtti() == Object::POINT) {
			return true;
		}
	}
	return false;
}

bool MovePointMode::isObjectSelectable(ObjectPtr object) const {
	PointPtr point = object;
	return (!point.isNull() && point->canMove());
}

void MovePointMode::onMouseRelease(int x, int y) {
	if (!myPoint.isNull()) {
		document()->movePoint(myPoint, x, y, true);
	  unselect(myPoint);
	  myPoint.reset();
	  repaintView();
	}
}

void MovePointMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(myPoint);
	unselect(myPoint); myPoint.reset();
	myPoint = closestObject(x, y);
	if (!myPoint.isNull()) {
		doRepaint = true;
		select(myPoint);
	}
	if (doRepaint) {
		repaintView();
	}
}

void MovePointMode::onMousePressedMove(int x, int y) {
	if (!myPoint.isNull()) {
		document()->movePoint(myPoint, x, y, false);
	  repaintView();
	}
}
