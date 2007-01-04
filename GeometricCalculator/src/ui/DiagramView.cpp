#include <math.h>

#include <ZLOptions.h>

#include "DiagramView.h"
#include "DrawableObject.h"
#include "EditMode.h"
#include "LineCoords.h"
#include "../document/Document.h"
#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "../model/Circle.h"

const std::string &DiagramView::caption() const {
	static const std::string EMPTY;
	shared_ptr<Scene> scene = document()->scene();
	return scene.isNull() ? EMPTY : scene->name();
}

static const std::string POINT_UI_OPTIONS = "Point";
static const std::string COLOR_OPTIONS = "Color";
static const std::string DRAWING_OPTIONS = "Drawing";

DiagramView::DiagramView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context),
	CreateSegmentForMiddlePointOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "Drawing", "CreateSegmentForMiddlePoint", false),
	ExistingPointsOnlyOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "Drawing", "ExistingPointsOnly", true),
	PointRadiusOption(ZLOption::LOOK_AND_FEEL_CATEGORY, POINT_UI_OPTIONS, "PointRadius", 1, 40, 5),
	ShowPointNamesOption(ZLOption::LOOK_AND_FEEL_CATEGORY, POINT_UI_OPTIONS, "ShowPointNames", true),
	PointFontFamilyOption(ZLOption::LOOK_AND_FEEL_CATEGORY, POINT_UI_OPTIONS, "FontFamily", "Helvetica"),
	PointFontSizeOption(ZLOption::LOOK_AND_FEEL_CATEGORY, POINT_UI_OPTIONS, "FontSize", 8, 72, 20),
	BackgroundColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLOR_OPTIONS, "Background", ZLColor(255, 255, 255)),
	ActiveColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLOR_OPTIONS, "Active", ZLColor(0, 0, 0)),
	InactiveColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLOR_OPTIONS, "Inactive", ZLColor(95, 95, 95)),
	SelectedColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLOR_OPTIONS, "Selected", ZLColor(0, 0, 255)),
	RulerColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLOR_OPTIONS, "Ruler", ZLColor(191, 0, 0)),
	ZoomOption(ZLOption::LOOK_AND_FEEL_CATEGORY, "View", "Zoom", 10, 500, 100) {
	shared_ptr<Scene> scene = new Scene();
	myDocument = new Document(scene);

	reset();
}

void DiagramView::reset() {
	mySelected.clear();
}

void DiagramView::setEditMode(shared_ptr<EditMode> mode) {
	if (myCurrentMode != mode) {
		if (!myCurrentMode.isNull()) {
			myCurrentMode->release();
		}
		myCurrentMode = mode;
		reset();
		repaintView();
	}
}

DiagramView::~DiagramView() {
	myDocument->removeAllTemporaryElements();
	delete myDocument;
}

Document *DiagramView::document() const {
	return myDocument;
}

DiagramView::DrawMode DiagramView::getDrawMode(const ObjectPtr object) const {
	if (document()->isTemporary(object)) {
		return isSelected(object) ? PREREGULAR : TEMPORARY;
	}

	if (isSelected(object)) {
		return SELECTED;
	}

	if (!myCurrentMode.isNull() && !myCurrentMode->isObjectSelectable(object)) {
		return UNAVAILABLE;
	}

	return REGULAR;
}

void DiagramView::draw() {
	const std::list<ObjectPtr> &objects = document()->scene()->objects();
  for (std::list<ObjectPtr>::const_iterator oit = objects.begin(); oit != objects.end(); oit++) {
		drawObject(*oit);
	}

	const std::set<ObjectPtr> &temporaryObjects = document()->temporaryObjects();
  for (std::set<ObjectPtr>::const_iterator ojt = temporaryObjects.begin(); ojt != temporaryObjects.end(); ojt++) {
		drawObject(*ojt);
	}

	const std::list<ValuePtr> &values = document()->scene()->values();
  for (std::list<ValuePtr>::const_iterator vit = values.begin(); vit != values.end(); vit++) {
		ValuePtr value = *vit;
		Distance &dist = (Distance&)*value;
		if (!document()->scene()->isPersistent(dist.startPoint())) {
			continue;
		}
		if (!document()->scene()->isPersistent(dist.endPoint())) {
			continue;
		}
		drawRuler(value);
	}

	const std::set<ValuePtr> &temporaryValues = document()->temporaryValues();
  for (std::set<ValuePtr>::const_iterator vjt = temporaryValues.begin(); vjt != temporaryValues.end(); vjt++) {
		ValuePtr value = *vjt;
		const Distance &dist = (Distance&)*value;
		const PointPtr start = dist.startPoint();
		if (!document()->scene()->isPersistent(start) && !document()->isTemporary(start)) {
			continue;
		}
		const PointPtr end = dist.endPoint();
		if (!document()->scene()->isPersistent(end) && !document()->isTemporary(end)) {
			continue;
		}
		drawRuler(value);
	}
}

void DiagramView::drawObject(const ObjectPtr object) {
	if (!object->isReal()) {
		return;
	}
	if (object->rtti() == Object::POINT) {
		PointPtr point = object;
		drawPoint(point, getDrawMode(object));
	} else if (object->rtti() == Object::LINE) {
		LinePtr line = object;
		LineCoordsPtr coords = new LineCoords(line, unzoomed(context().width()), unzoomed(context().height()));
		drawLine(coords, getDrawMode(object));
		if (line->kind() == LINE) {
			return;
		}

		PointPtr startPoint, endPoint;
		double startAlpha = 0.5, endAlpha = 0.5;
		Coordinates start = line->point(START)->coordinates();
		Coordinates end = line->point(END)->coordinates();
		double dx = start.x - end.x;
		double dy = start.y - end.y;
		bool useX = fabs(dx) > fabs(dy);

		const std::list<PointWeakPtr> &points = line->points();
		for (std::list<PointWeakPtr>::const_iterator pointIt = points.begin(); pointIt != points.end(); pointIt++) {
			if ((*pointIt).isNull()) {
				continue;
			}

			PointPtr point = *pointIt;

			if (!document()->scene()->isPersistent(point) && !document()->isTemporary(point)) {
				continue;
			}

			if (!point->isReal()) {
				continue;
			}

			double alpha = useX ?
				(start.x - point->coordinates().x) / dx :
				(start.y - point->coordinates().y) / dy;
  
			if ((alpha >= 0) && (alpha <= 1)) {
				continue;
			}
  
			if (alpha > 1) {
				if (line->kind() == HALFLINE) {
					continue;
				}
				if (endAlpha < alpha) {
					endAlpha = alpha;
					endPoint = point;
				}
			} else {
				if (startAlpha > alpha) {
					startAlpha = alpha;
					startPoint = point;
				}
			}
		}

		if (!startPoint.isNull() || !endPoint.isNull()) {
			DrawMode drawMode = REGULAR_AUX;
			if (isSelected(line)) {
				drawMode = SELECTED_AUX;
			} else if ((!myCurrentMode.isNull()) && !myCurrentMode->isObjectSelectable(line)) {
				drawMode = UNAVAILABLE_AUX;
			}
			if (!startPoint.isNull()) {
				LineCoordsPtr coords = new LineCoords(line->point(START), startPoint);
				drawLine(coords, drawMode);
			}
			if (!endPoint.isNull()) {
				LineCoordsPtr coords = new LineCoords(line->point(END), endPoint);
				drawLine(coords, drawMode);
			}
		}
	} else if (object->rtti() == Object::CIRCLE) {
		CirclePtr circle = object;
		drawCircle(circle, getDrawMode(object));
	}
}

void DiagramView::select(const ObjectPtr object) {
	if (!object.isNull() && !isSelected(object)) {
		mySelected.insert(object);
	}
}

void DiagramView::unselect(const ObjectPtr object) {
	if (!object.isNull() && isSelected(object)) {
		mySelected.erase(object);
	}
}

bool DiagramView::isSelected(const ObjectPtr object) const {
	return mySelected.find(object) != mySelected.end();
}

void DiagramView::drawPoint(const PointPtr point, DrawMode drawMode) {
	const Coordinates coords = point->coordinates();
	const int x = (int)(coords.x + .5);
	const int y = (int)(coords.y + .5);
	const std::string &name = point->name();

	int level;
	ZLColor color;
	bool solid = true;

	switch (drawMode) {
		case REGULAR:
		case REGULAR_AUX:
			color = ActiveColorOption.value();
			level = 50;
			break;
		case UNAVAILABLE:
		case UNAVAILABLE_AUX:
			color = InactiveColorOption.value();
			level = 45;
			break;
		case SELECTED:
		case SELECTED_AUX:
		case PREREGULAR:
			color = SelectedColorOption.value();
			level = 65;
			break;
		case TEMPORARY:
		default:
			color = SelectedColorOption.value();
			solid = false;
			level = 60;
			break;
	}
	const int radius = PointRadiusOption.value();
	addDrawableObject(new DrawablePoint(zoomed(x), zoomed(y), radius, color, solid), level);
	if (ShowPointNamesOption.value() && (name.length() > 0)) {
		addDrawableObject(new DrawableText(zoomed(x) + radius / 2, zoomed(y) - 3 * radius, name, PointFontFamilyOption.value(), PointFontSizeOption.value(), color), level);
	}
}

void DiagramView::drawLine(const LineCoordsPtr line, DrawMode drawMode) {
	int level;
	ZLColor color;
	bool solid = true;

	switch (drawMode) {
		case REGULAR:
			color = ActiveColorOption.value();
			level = 30;
			break;
		case REGULAR_AUX:
			color = ActiveColorOption.value();
			solid = false;
			level = 25;
			break;
		case UNAVAILABLE:
			color = InactiveColorOption.value();
			level = 35;
			break;
		case UNAVAILABLE_AUX:
			color = InactiveColorOption.value();
			solid = false;
			level = 35;
			break;
		case SELECTED:
		case PREREGULAR:
			color = SelectedColorOption.value();
			level = 40;
			break;
		case SELECTED_AUX:
		case TEMPORARY:
		default:
			color = SelectedColorOption.value();
			solid = false;
			level = 40;
			break;
	}
	addDrawableObject(new DrawableLine(
		zoomed(line->XStart()), zoomed(line->YStart()),
		zoomed(line->XEnd()), zoomed(line->YEnd()),
		color, solid), level
	);
}

void DiagramView::drawCircle(const CirclePtr circle, DrawMode drawMode) {
	ZLColor color;
	bool solid = true;
	int level = 25;

	switch (drawMode) {
		case REGULAR:
			color = ActiveColorOption.value();
			level = 30;
			break;
		case REGULAR_AUX:
			color = ActiveColorOption.value();
			solid = false;
			level = 25;
			break;
		case UNAVAILABLE:
			color = InactiveColorOption.value();
			level = 35;
			break;
		case UNAVAILABLE_AUX:
			color = InactiveColorOption.value();
			solid = false;
			level = 35;
			break;
		case SELECTED:
		case PREREGULAR:
			color = SelectedColorOption.value();
			level = 40;
			break;
		case SELECTED_AUX:
		case TEMPORARY:
		default:
			color = SelectedColorOption.value();
			solid = false;
			level = 40;
			break;
	}

	const Coordinates center = circle->center();

	addDrawableObject(new DrawableCircle(zoomed(center.x), zoomed(center.y), zoomed(circle->radius()), color, solid), level);
}

void DiagramView::drawRuler(const ValuePtr ruler) {
	Coordinates start = ((Distance&)*ruler).startPoint()->coordinates();
	Coordinates end = ((Distance&)*ruler).endPoint()->coordinates();

	addDrawableObject(new DrawableRuler(
		zoomed((int)(start.x + .5)),
		zoomed((int)(start.y + .5)),
		zoomed((int)(end.x + .5)),
		zoomed((int)(end.y + .5)),
		RulerColorOption.value(), true),
	42);
}

void DiagramView::paint() {
	draw();

	context().clear(BackgroundColorOption.value());

	for (int i = 0; i < DRAWABLE_LEVELS_NUMBER; i++) {
		std::vector<DrawableObject*> &objects = myDrawableObjects[i];
		for (std::vector<DrawableObject*>::iterator it = objects.begin(); it != objects.end(); it++) {
			(*it)->draw(context());
			delete *it;
		}
		objects.clear();
	}
}

bool DiagramView::onStylusPress(int x, int y) {
	if (!myCurrentMode.isNull()) {
		myCurrentMode->onMousePress(unzoomed(x), unzoomed(y));
 	}
	return true;
}

bool DiagramView::onStylusRelease(int x, int y) {
	if (!myCurrentMode.isNull()) {
		myCurrentMode->onMouseRelease(unzoomed(x), unzoomed(y));
 	}
	return true;
}

bool DiagramView::onStylusMove(int x, int y) {
	if (!myCurrentMode.isNull()) {
		myCurrentMode->onMouseMove(unzoomed(x), unzoomed(y));
 	}
	return true;
}

bool DiagramView::onStylusMovePressed(int x, int y) {
	if (!myCurrentMode.isNull()) {
		myCurrentMode->onMousePressedMove(unzoomed(x), unzoomed(y));
 	}
	return true;
}

void DiagramView::addDrawableObject(DrawableObject *object, int level) {
	myDrawableObjects[std::min(std::max(level, 0), DRAWABLE_LEVELS_NUMBER - 1)].push_back(object);
}

int DiagramView::zoomed(int coordinate) const {
	return coordinate * ZoomOption.value() / 100;
}

double DiagramView::zoomed(double coordinate) const {
	return coordinate * ZoomOption.value() / 100;
}

int DiagramView::unzoomed(int coordinate) const {
	return coordinate * 100 / ZoomOption.value();
}

double DiagramView::unzoomed(double coordinate) const {
	return coordinate * 100 / ZoomOption.value();
}
