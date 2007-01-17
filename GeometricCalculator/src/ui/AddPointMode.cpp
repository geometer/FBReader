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
		((object->rtti() == Object::LINE) || (object->rtti() == Object::CIRCLE));
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
