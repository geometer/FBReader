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
