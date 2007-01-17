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
		if (rtti == Object::POINT) {
			if (lineFlag) {
				return true;
			}
			pointFlag = true;
		} else if ((rtti == Object::LINE) || (rtti == Object::CIRCLE)) {
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
		return (rtti == Object::LINE) || (rtti == Object::CIRCLE);
	} else {
	  if (object == mySelectedObject0) {
			return false;
		}
		if (view().ExistingPointsOnlyOption.value() && (object->rtti() != Object::POINT)) {
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
