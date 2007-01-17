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

#ifndef __DIAGRAMVIEW_H__
#define __DIAGRAMVIEW_H__

#include <vector>
#include <set>
#include <map>

#include <ZLView.h>
#include <ZLPaintContext.h>
#include <ZLOptions.h>

#include "../model/Scene.h"
#include "LineCoords.h"

class Document;
class PointPtr;
class CirclePtr;
class DrawableObject;
class EditMode;

class DiagramView : public ZLView {

public:
	ZLBooleanOption CreateSegmentForMiddlePointOption;
	ZLBooleanOption ExistingPointsOnlyOption;
	ZLIntegerRangeOption PointRadiusOption;
	ZLBooleanOption ShowPointNamesOption;
	ZLStringOption PointFontFamilyOption;
	ZLIntegerRangeOption PointFontSizeOption;
	ZLColorOption BackgroundColorOption;
	ZLColorOption ActiveColorOption;
	ZLColorOption InactiveColorOption;
	ZLColorOption SelectedColorOption;
	ZLColorOption RulerColorOption;
	ZLIntegerRangeOption ZoomOption;

public:
	DiagramView(ZLApplication &application, ZLPaintContext &context);
	~DiagramView();

	Document *document() const;
	void setEditMode(shared_ptr<EditMode> mode);
	const std::string &caption() const;

	void paint();
  bool onStylusPress(int x, int y);
  bool onStylusRelease(int x, int y);
  bool onStylusMove(int x, int y);
  bool onStylusMovePressed(int x, int y);

	void draw();

private:
	enum DrawMode {
		REGULAR,
		REGULAR_AUX,
		UNAVAILABLE,
		UNAVAILABLE_AUX,
		SELECTED,
		SELECTED_AUX,
		TEMPORARY,
		PREREGULAR
	};

	enum DrawableObjectLevel {
		ACTIVE_AUXILARY_LINE_LEVEL = 25,
		ACTIVE_LINE_LEVEL = 30,
		INACTIVE_LINE_LEVEL = 35,
		SELECTED_LINE_LEVEL = 40,
		RULER_LEVEL = 42,
		INACTIVE_POINT_LEVEL = 45,
		ACTIVE_POINT_LEVEL = 50,
		TEMPORARY_POINT_LEVEL = 60,
		SELECTED_POINT_LEVEL = 65,
	};

	void drawPoint(const PointPtr point, DrawMode drawMode);
	void drawLine(const LineCoordsPtr line, DrawMode drawMode);
	void drawCircle(const CirclePtr circle, DrawMode drawMode);
	void drawRuler(const ValuePtr ruler);

	void addDrawableObject(DrawableObject *object, DrawableObjectLevel level);

private:
	DrawMode getDrawMode(const ObjectPtr object) const;
	void drawObject(const ObjectPtr object);
	bool isSelected(const ObjectPtr object) const;
	void select(const ObjectPtr object);
	void unselect(const ObjectPtr object);
	void reset();

	int zoomed(int coordinate) const;
	double zoomed(double coordinate) const;
	int unzoomed(int coordinate) const;
	double unzoomed(double coordinate) const;

private:
	typedef std::map<DrawableObjectLevel,std::vector<DrawableObject*> > DrawableObjectMap;
	DrawableObjectMap myDrawableObjects;
	shared_ptr<EditMode> myCurrentMode;
	Document *myDocument;
	std::set<ObjectPtr> mySelected;

friend class EditMode;
};

#endif /* __DIAGRAMVIEW_H__ */
