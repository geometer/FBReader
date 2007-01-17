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
		if ((*it)->rtti() == Object::POINT) {
			if (lineFlag) {
				return true;
			}
			pointFlag = true;
		} else if ((*it)->rtti() == Object::LINE) {
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
		return object->rtti() == Object::LINE;
	} else {
		if (view().ExistingPointsOnlyOption.value() && (object->rtti() != Object::POINT)) {
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
