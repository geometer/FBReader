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

#include "AddPointOnTheLineMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

AddPointOnTheLineMode::AddPointOnTheLineMode(DiagramView &view) : AddPointBaseMode(view) {
}

bool AddPointOnTheLineMode::isAvailable() const {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if ((*it)->rtti() == Object::LINE) {
			return true;
		}
	}
	return false;
}

bool AddPointOnTheLineMode::isObjectSelectable(ObjectPtr object) const {
	if (mySelectedObject0.isNull()) {
		return object->rtti() == Object::LINE;
	} else {
		return mySelectedObject1.isNull() && (object != mySelectedObject0) &&
			((object->rtti() == Object::LINE) || (object->rtti() == Object::CIRCLE));
	}
}

void AddPointOnTheLineMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedObject0);
	unselect(mySelectedObject0); mySelectedObject0.reset();
	mySelectedObject0 = closestObject(x, y);
	if (!mySelectedObject0.isNull()) {
		doRepaint = true;
		select(mySelectedObject0);
	}
	if (doRepaint) {
		repaintView();
	}
}

void AddPointOnTheLineMode::onMousePress(int x, int y) {
	if (!mySelectedObject0.isNull()) {
		unselect(mySelectedObject1); mySelectedObject1.reset();
		document()->remove(myPoint);
		unselect(myPoint); myPoint.reset();

		myPoint = getPoint(mySelectedObject0, mySelectedObject1, x, y, true);
		myPoint->moveTo(x, y);
		mySelectedObject1 = closestObject(myPoint->coordinates());
		if (!mySelectedObject1.isNull()) {
			PointPtr point = getPoint(mySelectedObject0, mySelectedObject1, x, y, true);
			if (point->isReal() && (myPoint->distance(point) <= 7)) {
				select(mySelectedObject1);
				myPoint = point;
			}
		}
		select(myPoint);
		document()->addTemporary(myPoint);

		repaintView();
	}
}
