#include "AddPerpendicularMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Line.h"

AddPerpendicularMode::AddPerpendicularMode(DiagramView &view) : AddConstructMode(view) {
}

void AddPerpendicularMode::selectNewObjects() {
	AddConstructMode::selectNewObjects();
	select(myCrossingPoint);
}

void AddPerpendicularMode::unselectNewObjects() {
	AddConstructMode::unselectNewObjects();
	unselect(myCrossingPoint);
}

void AddPerpendicularMode::release() {
	AddConstructMode::release();
	myCrossingPoint.reset();
}

bool AddPerpendicularMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	bool lineFlag = false, pointFlag = false;
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if ((*it)->rtti() == Point::RTTI) {
			if (lineFlag) {
				return true;
			}
			pointFlag = true;
		} else if ((*it)->rtti() == Line::RTTI) {
			if (view().ExistingPointsOnlyOption.value()) {
				if (pointFlag) {
					return true;
				}
				lineFlag = true;
			} else {
				return true;
			}
		}
	}
	return false;
}

bool AddPerpendicularMode::isObjectSelectable(ObjectPtr object) const {
	if (myMousePoint.isNull()) {
		return object->rtti() == Line::RTTI;
	} else {
		if (view().ExistingPointsOnlyOption.value() && (object->rtti() != Point::RTTI)) {
			return false;
		}
		return object != mySelectedObject0;
	}
}

void AddPerpendicularMode::onMousePress(int x, int y) {
	if (!mySelectedObject.isNull()) {
		myMousePoint = IndependentPoint::create(x, y);
		document()->addTemporary(myMousePoint);

		myInvisiblePoint = PointOnPerpendicular::create(mySelectedObject, myMousePoint);
		myNewObject = LineByTwoPoints::create(myMousePoint, myInvisiblePoint, LINE);
		document()->addTemporary(myNewObject);

		myCrossingPoint = LinesCrossing::create(mySelectedObject, myNewObject);
		document()->addTemporary(myCrossingPoint);

		onMousePressedMove(x, y);
	}
}

void AddPerpendicularMode::onMouseRelease(int x, int y) {
	unselectNewObjects();
	unselect(mySelectedObject);
	if (!mySelectedObject.isNull() && !myNewObject.isNull()) {
		document()->removeAllTemporaryElements();
		myMousePoint = getPoint(x, y, !view().ExistingPointsOnlyOption.value());
		if (!myMousePoint.isNull()) {
			myInvisiblePoint = PointOnPerpendicular::create(mySelectedObject, myMousePoint);
			myNewObject = LineByTwoPoints::create(myMousePoint, myInvisiblePoint, LINE);
			document()->startTransaction();
			if (myMousePoint != mySelectedObject0) {
				document()->add(myMousePoint);
			}
			document()->add(myNewObject);
			if (!((Line&)*mySelectedObject).contains(myMousePoint)) {
				myCrossingPoint = LinesCrossing::create(mySelectedObject, myNewObject);
				document()->add(myCrossingPoint);
			}
			document()->endTransaction();
		}

	  release();

		repaintView();
	}
}
