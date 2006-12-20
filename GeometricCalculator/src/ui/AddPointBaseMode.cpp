#include "AddPointBaseMode.h"

#include "../document/Document.h"

#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

AddPointBaseMode::AddPointBaseMode(DiagramView &view) : BaseAddMode(view) {
}

void AddPointBaseMode::release() {
	myPoint.reset();
	mySelectedObject0.reset();
	mySelectedObject1.reset();
}

void AddPointBaseMode::onMousePressedMove(int x, int y) {
	if (!myPoint.isNull()) {
		onMousePress(x, y);
	}
}

void AddPointBaseMode::onMouseRelease(int, int) {
	if (!myPoint.isNull()) {
		document()->remove(myPoint);
		unselect(myPoint);
		unselect(mySelectedObject0);
		unselect(mySelectedObject1);
		document()->add(myPoint);
		release();
		repaintView();
	}
}
