#include "EraseMode.h"
#include "../document/Document.h"
#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Line.h"

EraseMode::EraseMode(DiagramView &view) : EditMode(view) {
}

void EraseMode::release() {
	myObject.reset();
}

bool EraseMode::isAvailable() const {
	return document()->scene()->objects().size() > 0;
}

bool EraseMode::isObjectSelectable(ObjectPtr) const {
	return true;
}

void EraseMode::onMousePress(int, int) {
	if (!myObject.isNull()) {
		document()->remove(myObject);
		unselect(myObject);
		myObject.reset();
		repaintView();
	}
}

void EraseMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(myObject);
	unselect(myObject); myObject.reset();
	myObject = closestObject(x, y);
	if (!myObject.isNull()) {
		doRepaint = true;
		select(myObject);
	}
	if (doRepaint) {
		repaintView();
	}
}
