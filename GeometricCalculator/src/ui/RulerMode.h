#ifndef __RULERMODE_H__
#define __RULERMODE_H__

#include "EditMode.h"
#include "../model/Point.h"
#include "../model/Circle.h"

class RulerMode : public EditMode {
	private:
		PointPtr mySelectedPoint0;
		PointPtr mySelectedPoint1;
		PointPtr myInvisiblePoint;
		ValuePtr myRuler;

	public:
		RulerMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMousePress(int x, int y);
		void onMouseRelease(int x, int y);
		void onMouseMove(int x, int y);
		void onMousePressedMove(int x, int y);
};

#endif /* __RULERMODE_H__ */
