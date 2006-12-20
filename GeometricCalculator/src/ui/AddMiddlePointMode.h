#ifndef __ADDMIDDLEPOINTMODE_H__
#define __ADDMIDDLEPOINTMODE_H__

#include "EditMode.h"
#include "../model/Point.h"
#include "../model/Line.h"

class AddMiddlePointMode : public EditMode {
	private:
		PointPtr mySelectedPoint;
		PointPtr myMousePoint;
		PointPtr myMiddlePoint;
		LinePtr myTemporaryLine;
		LinePtr mySelectedLine;

		LinePtr findLine(PointPtr point0, PointPtr point1) const;

	public:
		AddMiddlePointMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMousePress(int x, int y);
		void onMouseRelease(int x, int y);
		void onMouseMove(int x, int y);
		void onMousePressedMove(int x, int y);
};

#endif /* __ADDMIDDLEPOINTMODE_H__ */
