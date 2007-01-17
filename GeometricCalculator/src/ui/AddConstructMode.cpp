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
