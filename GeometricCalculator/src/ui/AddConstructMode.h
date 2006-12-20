#ifndef __ADDCONSTRUCTMODE_H__
#define __ADDCONSTRUCTMODE_H__

#include "BaseAddMode.h"
#include "../model/Point.h"

class AddConstructMode : public BaseAddMode {
	protected:
		AddConstructMode(DiagramView &view);
		virtual void selectNewObjects();
		virtual void unselectNewObjects();

	public:
		virtual void release();

		void onMouseMove(int x, int y);
		void onMousePressedMove(int x, int y);

	protected:
		PointPtr myMousePoint;
		PointPtr myInvisiblePoint;

		ObjectPtr myNewObject;
		ObjectPtr mySelectedObject;
};

#endif /* __ADDCONSTRUCTMODE_H__ */
