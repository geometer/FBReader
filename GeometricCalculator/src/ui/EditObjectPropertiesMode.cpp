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

#include <ZLDialog.h>
#include <ZLDialogManager.h>
#include <ZLOptionEntry.h>

#include "EditObjectPropertiesMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Circle.h"

class PointNameEntry : public ZLStringOptionEntry {

public:
	PointNameEntry(Point &point);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Point &myPoint;
};

PointNameEntry::PointNameEntry(Point &point) : myPoint(point) {
}

const std::string &PointNameEntry::name() const {
	static const std::string NAME = "Name";
	return NAME;
}

const std::string &PointNameEntry::initialValue() const {
	return myPoint.name();
}

void PointNameEntry::onAccept(const std::string &value) {
	myPoint.setName(value);
}

EditObjectPropertiesMode::EditObjectPropertiesMode(DiagramView &view) : EditMode(view), myDialogRuns(false) {
}

void EditObjectPropertiesMode::release() {
	unselect(mySelectedObject);
	mySelectedObject.reset();
}

bool EditObjectPropertiesMode::isAvailable() const {
	return !document()->scene()->isEmpty();
}

bool EditObjectPropertiesMode::isObjectSelectable(ObjectPtr object) const {
	// TODO: replace by true
	return object->rtti() == Object::POINT;
}

void EditObjectPropertiesMode::onMousePress(int, int) {
	if (!mySelectedObject.isNull()) {
		PointPtr point = mySelectedObject;
		if (!point.isNull()) {
			shared_ptr<ZLDialog> propertiesDialog = ZLDialogManager::instance().createDialog(ZLResourceKey("pointDialog"));
			propertiesDialog->addOption(ZLResourceKey("name"), new PointNameEntry(*point));
			propertiesDialog->addButton(ZLDialogManager::OK_BUTTON, true);
			propertiesDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
			myDialogRuns = true;
			if (propertiesDialog->run()) {
				propertiesDialog->acceptValues();
			}
			myDialogRuns = false;
		}
		unselect(mySelectedObject);
		mySelectedObject.reset();
		repaintView();
	}
}

void EditObjectPropertiesMode::onMouseRelease(int, int) {
	if (!mySelectedObject.isNull() && !myDialogRuns) {
		unselect(mySelectedObject);
		mySelectedObject.reset();
		repaintView();
	}
}

void EditObjectPropertiesMode::onMouseMove(int x, int y) {
	bool doRepaint = isSelected(mySelectedObject);
	unselect(mySelectedObject);
	mySelectedObject.reset();
	mySelectedObject = closestObject(x, y);
	if (!mySelectedObject.isNull()) {
		doRepaint = true;
		select(mySelectedObject);
	}
	if (doRepaint) {
		repaintView();
	}
}
