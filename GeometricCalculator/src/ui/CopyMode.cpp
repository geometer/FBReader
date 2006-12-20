#include "CopyMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

CopyMode::CopyMode(DiagramView &view) : AddConstructMode(view) {
}

bool CopyMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	bool lineFlag = false, pointFlag = false;
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		int rtti = (*it)->rtti();
		if (rtti == Point::RTTI) {
			if (lineFlag) {
				return true;
			}
			pointFlag = true;
		} else if ((rtti == Line::RTTI) || (rtti == Circle::RTTI)) {
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

bool CopyMode::isObjectSelectable(ObjectPtr object) const {
	if (myNewObject.isNull()) {
		int rtti = object->rtti();
		return (rtti == Line::RTTI) || (rtti == Circle::RTTI);
	} else {
	  if (object == mySelectedObject0) {
			return false;
		}
		if (view().ExistingPointsOnlyOption.value() && (object->rtti() != Point::RTTI)) {
			return false;
		}
		LinePtr line = mySelectedObject;
		if (!line.isNull()) {
			return object != line->point(mySamePointOrder ? START : END);
		}
		CirclePtr circle = mySelectedObject;
		if (!circle.isNull()) {
			return object != circle->centerPoint();
		}
		return true;
	}
}

void CopyMode::onMousePress(int x, int y) {
	if (!mySelectedObject.isNull()) {
		myMousePoint = IndependentPoint::create(x, y);
		document()->addTemporary(myMousePoint);

		LinePtr line = mySelectedObject;
		if (!line.isNull()) {
			LineKind kind = line->kind();
			mySamePointOrder =
				(kind != SEGMENT) ||
		  	(myMousePoint->distance(line->point(START)) < myMousePoint->distance(line->point(END)));
			myInvisiblePoint =
				FourthParallelogramPoint::create(
					myMousePoint,
					line->point(mySamePointOrder ? START : END),
					line->point(mySamePointOrder ? END : START)
				);
			myNewObject = LineByTwoPoints::create(myMousePoint, myInvisiblePoint, kind);
			if (kind == SEGMENT) {
				document()->addTemporary(myInvisiblePoint);
			}
		}
		
		CirclePtr circle = mySelectedObject;
		if (!circle.isNull()) {
			myNewObject = CircleCopy::create(myMousePoint, circle);
		}

		document()->addTemporary(myNewObject);

		onMousePressedMove(x, y);
	}
}

void CopyMode::onMouseRelease(int x, int y) {
	unselectNewObjects();
	unselect(mySelectedObject);
	if (!myNewObject.isNull()) {
		document()->removeAllTemporaryElements();
		myMousePoint = getPoint(x, y, !view().ExistingPointsOnlyOption.value());
		if (!myMousePoint.isNull()) {
			document()->startTransaction();
			if (myMousePoint != mySelectedObject0) {
				document()->add(myMousePoint);
			}

			LinePtr line = mySelectedObject;
			if (!line.isNull()) {
				LineKind kind = line->kind();
				myInvisiblePoint =
					FourthParallelogramPoint::create(
						myMousePoint,
						line->point(mySamePointOrder ? START : END),
						line->point(mySamePointOrder ? END : START)
					);
				myNewObject = LineByTwoPoints::create(myMousePoint, myInvisiblePoint, kind);
				if (kind == SEGMENT) {
					document()->add(myInvisiblePoint);
				}
			}

			CirclePtr circle = mySelectedObject;
			if (!circle.isNull()) {
				myNewObject = CircleCopy::create(myMousePoint, circle);
			}

			document()->add(myNewObject);
			document()->endTransaction();
		}

	  release();

		repaintView();
	}
}
