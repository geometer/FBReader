/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
