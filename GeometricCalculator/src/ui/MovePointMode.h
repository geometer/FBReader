#ifndef __MOVEPOINTMODE_H__
#define __MOVEPOINTMODE_H__

#include "EditMode.h"
#include "../model/Point.h"

class MovePointMode : public EditMode {
	private:
		PointPtr myPoint;

	public:
		MovePointMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMouseRelease(int x, int y);
		void onMouseMove(int x, int y);
		void onMousePressedMove(int x, int y);
};

#endif /* __MOVEPOINTMODE_H__ */
