#ifndef __DIAGRAMVIEW_H__
#define __DIAGRAMVIEW_H__

#include <vector>
#include <set>

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

const int DRAWABLE_LEVELS_NUMBER = 100;

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

	void drawPoint(const PointPtr point, DrawMode drawMode);
	void drawLine(const LineCoordsPtr line, DrawMode drawMode);
	void drawCircle(const CirclePtr circle, DrawMode drawMode);
	void drawRuler(const ValuePtr ruler);

	void addDrawableObject(DrawableObject *object, int level);

private:
	DrawMode getDrawMode(const ObjectPtr object) const;
	void drawObject(const ObjectPtr object);
	bool isSelected(const ObjectPtr object) const;
	void select(const ObjectPtr object);
	void unselect(const ObjectPtr object);
	void reset();

	int zoomed(int coordinate) const;
	int unzoomed(int coordinate) const;

private:
	std::vector<DrawableObject*> myDrawableObjects[DRAWABLE_LEVELS_NUMBER];
	shared_ptr<EditMode> myCurrentMode;
	Document *myDocument;
	std::set<ObjectPtr> mySelected;

friend class EditMode;
};

#endif /* __DIAGRAMVIEW_H__ */
