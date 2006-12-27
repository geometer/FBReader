#include "RulerMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Circle.h"

RulerMode::RulerMode(DiagramView &view) : EditMode(view) {
}

void RulerMode::release() {
	mySelectedPoint0.reset();
	mySelectedPoint1.reset();
	myInvisiblePoint.reset();
	myRuler.reset();
}

bool RulerMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	int pointCounter = 0;
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if ((*it)->rtti() == Object::POINT) {
			pointCounter++;
		}
		if (pointCounter == 2) {
			return true;
		}
	}
	return false;
}

bool RulerMode::isObjectSelectable(ObjectPtr object) const {
	return (object->rtti() == Object::POINT) && (object != mySelectedPoint0);
}

void RulerMode::onMousePress(int x, int y) {
	if (!mySelectedPoint0.isNull()) {
		myInvisiblePoint = IndependentPoint::create(x, y);
		myRuler = new Distance(mySelectedPoint0, myInvisiblePoint);
		document()->addTemporary(myInvisiblePoint);
		document()->addTemporary(myRuler);
		onMousePressedMove(x, y);
		repaintView();
	}
}

void RulerMode::onMouseRelease(int, int) {
	unselect(mySelectedPoint0);
	unselect(mySelectedPoint1);
	document()->remove(myInvisiblePoint);
	document()->remove(myRuler);
	if (!mySelectedPoint1.isNull()) {
		myRuler = new Distance(mySelectedPoint0, mySelectedPoint1);
		document()->add(myRuler);
	}
	mySelectedPoint0.reset();
	mySelectedPoint1.reset();
	myInvisiblePoint.reset();
	myRuler.reset();
	repaintView();
}

void RulerMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedPoint0);
	unselect(mySelectedPoint0); mySelectedPoint0.reset();
	mySelectedPoint0 = closestObject(x, y);
	if (!mySelectedPoint0.isNull()) {
		doRepaint = true;
		select(mySelectedPoint0);
	}
	if (doRepaint) {
		repaintView();
	}
}

void RulerMode::onMousePressedMove(int x, int y) {
	if (!myRuler.isNull()) {
		unselect(mySelectedPoint1);
		mySelectedPoint1.reset();
		mySelectedPoint1 = closestObject(x, y);
		if (!mySelectedPoint1.isNull()) {
			select(mySelectedPoint1);
			myInvisiblePoint->moveTo(mySelectedPoint1->coordinates());
		} else {
			myInvisiblePoint->moveTo(x, y);
		}
		repaintView();
	}
}
