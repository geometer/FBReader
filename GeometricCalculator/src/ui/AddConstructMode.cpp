#include "AddConstructMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Line.h"

AddConstructMode::AddConstructMode(DiagramView &view) : BaseAddMode(view) {
}

void AddConstructMode::selectNewObjects() {
	select(myMousePoint);
	select(myInvisiblePoint);
	select(myNewObject);
}

void AddConstructMode::unselectNewObjects() {
	unselect(myMousePoint);
	unselect(myInvisiblePoint);
	unselect(myNewObject);
}

void AddConstructMode::release() {
	mySelectedObject0.reset();
	mySelectedObject1.reset();
	myMousePoint.reset();
	myInvisiblePoint.reset();
	mySelectedObject.reset();
	myNewObject.reset();
}

void AddConstructMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedObject);
	unselect(mySelectedObject);
	mySelectedObject = closestObject(x, y);
	if (!mySelectedObject.isNull()) {
		doRepaint = true;
		select(mySelectedObject);
	}
	if (doRepaint) {
		repaintView();
	}
}

void AddConstructMode::onMousePressedMove(int x, int y) {
	if (!myNewObject.isNull()) {
		PointPtr point = getPoint(x, y, !view().ExistingPointsOnlyOption.value());
		if (!point.isNull()) {
			select(mySelectedObject0);
			select(mySelectedObject1);
			myMousePoint->moveTo(point->coordinates());
			selectNewObjects();
		} else {
			myMousePoint->moveTo(x, y);
			unselectNewObjects();
		}

	  repaintView();
	}
}
