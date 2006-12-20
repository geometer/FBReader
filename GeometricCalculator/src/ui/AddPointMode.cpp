#include "AddPointMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

AddPointMode::AddPointMode(DiagramView &view) : AddPointBaseMode(view) {
}

bool AddPointMode::isAvailable() const {
	return true;
}

bool AddPointMode::isObjectSelectable(ObjectPtr object) const {
	return mySelectedObject1.isNull() && (object != mySelectedObject0) &&
		((object->rtti() == Line::RTTI) || (object->rtti() == Circle::RTTI));
}

void AddPointMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedObject0) || isSelected(mySelectedObject1);
	unselect(mySelectedObject0); mySelectedObject0.reset();
	unselect(mySelectedObject1); mySelectedObject1.reset();
	mySelectedObject0 = closestObject(x, y);
	if (!mySelectedObject0.isNull()) {
		doRepaint = true;
		select(mySelectedObject0);
		mySelectedObject1 = closestObject(x, y);
		select(mySelectedObject1);
	}
	if (doRepaint) {
		repaintView();
	}
}

void AddPointMode::onMousePress(int x, int y) {
	document()->remove(myPoint);
	unselect(myPoint); myPoint.reset();
	myPoint = getPoint(x, y, true);
	select(mySelectedObject0);
	select(mySelectedObject1);
	select(myPoint);
	document()->addTemporary(myPoint);
	repaintView();
}
