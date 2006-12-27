#include "AddMiddlePointMode.h"
#include "../document/Document.h"

AddMiddlePointMode::AddMiddlePointMode(DiagramView &view) : EditMode(view) {
}

void AddMiddlePointMode::release() {
	mySelectedPoint.reset();
	myMousePoint.reset();
	myMiddlePoint.reset();
	myTemporaryLine.reset();
	mySelectedLine.reset();
}

bool AddMiddlePointMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	int pointCounter = 0;
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if ((*it)->rtti() == Object::POINT) {
			pointCounter++;
			if (pointCounter == 2) {
				return true;
			}
		}
	}
	return false;
}

bool AddMiddlePointMode::isObjectSelectable(ObjectPtr object) const {
	return (object->rtti() == Object::POINT) && (object != mySelectedPoint);
}

void AddMiddlePointMode::onMousePress(int x, int y) {
	document()->remove(myMousePoint); unselect(myMousePoint); myMousePoint.reset();
	document()->remove(myMiddlePoint); unselect(myMiddlePoint); myMiddlePoint.reset();
	document()->remove(myTemporaryLine); unselect(myTemporaryLine); myTemporaryLine.reset();
	if (!mySelectedPoint.isNull()) {
		myMousePoint = IndependentPoint::create(x, y);
		document()->addTemporary(myMousePoint);
		if (view().CreateSegmentForMiddlePointOption.value()) {
			myTemporaryLine = LineByTwoPoints::create(mySelectedPoint, myMousePoint, SEGMENT);
			document()->addTemporary(myTemporaryLine);
		}
		myMiddlePoint = PointOnTheLine::create(mySelectedPoint, myMousePoint, myTemporaryLine, 0.5);
		document()->addTemporary(myMiddlePoint);
		repaintView();
	}
}

void AddMiddlePointMode::onMouseRelease(int x, int y) {
	document()->remove(myTemporaryLine);
	document()->remove(myMousePoint);
	document()->remove(myMiddlePoint);
	unselect(mySelectedLine);
	unselect(mySelectedPoint);
	unselect(myMousePoint);
	unselect(myMiddlePoint);
	unselect(myTemporaryLine);

	if (!myMousePoint.isNull()) {
		PointPtr point = closestObject(x, y);
		if (!point.isNull()) {
			LinePtr line = findLine(mySelectedPoint, point);
			bool addNewLine = false;
			if (view().CreateSegmentForMiddlePointOption.value() && line.isNull()) {
				addNewLine = true;
				line = LineByTwoPoints::create(mySelectedPoint, point, SEGMENT);
			}
			PointPtr middlePoint = PointOnTheLine::create(mySelectedPoint, point, line, 0.5);
			((PointOnTheLine&)*middlePoint).setFixed(true);
			document()->startTransaction();
			document()->add(middlePoint);
			if (addNewLine) {
				document()->add(line);
			}
			document()->endTransaction();
		}

		release();
	}

	repaintView();
}

void AddMiddlePointMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedPoint);
	unselect(mySelectedPoint);
	mySelectedPoint.reset();
	mySelectedPoint = closestObject(x, y);
	if (!mySelectedPoint.isNull()) {
		doRepaint = true;
		select(mySelectedPoint);
	}
	if (doRepaint) {
		repaintView();
	}
}

void AddMiddlePointMode::onMousePressedMove(int x, int y) {
	if (!myMousePoint.isNull()) {
		if (!mySelectedLine.isNull()) {
			unselect(mySelectedLine);
			document()->addTemporary(myTemporaryLine);
		}

		PointPtr point = closestObject(x, y);
		if (!point.isNull()) {
			myMousePoint->moveTo(point->coordinates());
			select(myMousePoint);
			select(myMiddlePoint);
			select(myTemporaryLine);
			if (view().CreateSegmentForMiddlePointOption.value()) {
				mySelectedLine = findLine(mySelectedPoint, point);
				if (!mySelectedLine.isNull()) {
					select(mySelectedLine);
					document()->remove(myTemporaryLine);
				}
			}
		} else {
			myMousePoint->moveTo(x, y);
			unselect(myMousePoint);
			unselect(myMiddlePoint);
			unselect(myTemporaryLine);
		}

		repaintView();
	}
}

LinePtr AddMiddlePointMode::findLine(PointPtr point0, PointPtr point1) const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		LinePtr line = *it;
		if (!line.isNull() && line->contains(point0) && line->contains(point1)) {
		  return *it;	
		}
	}
	return LinePtr();
}
