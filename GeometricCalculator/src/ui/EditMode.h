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

#ifndef __EDITMODE_H__
#define __EDITMODE_H__

#include "../model/Scene.h"
#include "DiagramView.h"

class Document;
class LinePtr;

class EditMode {

public:
	EditMode(DiagramView &view);
	virtual ~EditMode();

	virtual bool isAvailable() const = 0;
	virtual bool isObjectSelectable(ObjectPtr object) const = 0;
	virtual void release() = 0;

	virtual void onMousePress(int x, int y);
	virtual void onMouseRelease(int x, int y);
	virtual void onMouseMove(int x, int y);
	virtual void onMousePressedMove(int x, int y);

private:
	double distanceToLine(LinePtr line, double x, double y);

protected:
	Document *document() const;

	ObjectPtr closestObject(double x, double y);
	ObjectPtr closestObject(const Coordinates coords);
	bool isSelected(ObjectPtr object) const;
	void select(ObjectPtr object);
	void unselect(ObjectPtr object);

	const DiagramView &view() const;

	void repaintView();

private:
	DiagramView &myView;
};

inline EditMode::EditMode(DiagramView &view) : myView(view) {}

inline bool EditMode::isSelected(ObjectPtr object) const { return myView.isSelected(object); }
inline void EditMode::select(ObjectPtr object) { myView.select(object); }
inline void EditMode::unselect(ObjectPtr object) { myView.unselect(object); }

#endif /* __EDITMODE_H__ */
