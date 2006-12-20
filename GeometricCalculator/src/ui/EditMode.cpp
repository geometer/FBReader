#include <math.h>

#include "EditMode.h"
#include "DiagramView.h"
#include "../document/Document.h"
#include "../model/Object.h"
#include "../model/Point.h"
#include "../model/PointPtr.h"
#include "../model/Line.h"
#include "../model/LinePtr.h"
#include "../model/Scene.h"

EditMode::~EditMode() {
	document()->removeAllTemporaryElements();
}

void EditMode::onMousePress(int, int) {
}

void EditMode::onMouseRelease(int, int) {
}

void EditMode::onMouseMove(int, int) {
}

void EditMode::onMousePressedMove(int, int) {
}

Document *EditMode::document() const {
	return myView.document();
}

ObjectPtr EditMode::closestObject(const Coordinates coords) {
	return closestObject(coords.x, coords.y);
}

ObjectPtr EditMode::closestObject(double x, double y) {
	PointPtr point;
	ObjectPtr line;

	double maxPointDistance = 11;
	double maxLineDistance = 11;

  const std::list<ObjectPtr> &objects = document()->scene()->objects();
	for (std::list<ObjectPtr>::const_iterator it = objects.begin(); it != objects.end(); it++) {
		if (document()->isTemporary(*it) || !isObjectSelectable(*it)) {
			continue;
		}
		int rtti = (*it)->rtti();
		double distanceCandidate = (rtti == Line::RTTI) ? distanceToLine(*it, x, y) : (*it)->distance(x, y);

		if (rtti == Point::RTTI) {
			if (distanceCandidate < maxPointDistance) {
				point = *it;
				maxPointDistance = distanceCandidate;
			}
		} else {
			if (distanceCandidate < maxLineDistance) {
				line = *it;
				maxLineDistance = distanceCandidate;
			}
		}
  }

	if (!point.isNull()) {
		return point;
	} else {
		return line;
	}
}

double EditMode::distanceToLine(LinePtr line, double x, double y) {
	double lineDistance = line->distance(x, y);
	if (line->kind() == LINE) {
		return lineDistance;
	}
	if (line->points().empty()) {
		return lineDistance;
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

		if (!document()->scene()->isPersistent(point) || !point->isReal()) {
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

	if (startPoint.isNull() && endPoint.isNull()) {
		return lineDistance;
	}

	if (startPoint.isNull()) {
		startPoint = line->point(START);
	}

	if (endPoint.isNull()) {
		endPoint = line->point(END);
	}

	Coordinates coords = startPoint->coordinates();
	double segmentDistance, dx1, dy1;
	
	dx1 = coords.x - x;
	dy1 = coords.y - y;
	if (dx1 * dx + dy1 * dy < 0) {
	  segmentDistance = startPoint->distance(x, y);
		return (segmentDistance < lineDistance) ? segmentDistance : lineDistance;
	}

	coords = endPoint->coordinates();
	dx1 = x - coords.x;
	dy1 = y - coords.y;
	if (dx1 * dx + dy1 * dy < 0) {
	  segmentDistance = endPoint->distance(x, y);
		return (segmentDistance < lineDistance) ? segmentDistance : lineDistance;
	}

	double length = line->point(START)->distance(line->point(END));
	double doubledArea =
		fabs(x * start.y - y * start.x +
				 start.x * end.y - start.y * end.x +
				 end.x * y - end.y * x);
	segmentDistance = doubledArea / length;
	return (segmentDistance < lineDistance) ? segmentDistance : lineDistance;
}

void EditMode::repaintView() {
	myView.repaintView();
}

const DiagramView &EditMode::view() const {
	return myView;
}
