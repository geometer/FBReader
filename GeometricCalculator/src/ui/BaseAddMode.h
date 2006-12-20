#ifndef __BASEADDMODE_H__
#define __BASEADDMODE_H__

#include "EditMode.h"
#include "../model/Object.h"

class BaseAddMode : public EditMode {

protected:
	BaseAddMode(DiagramView &view);
	PointPtr getPoint(ObjectPtr object0, ObjectPtr object1, int x, int y, bool createNewPoint);
	PointPtr getPoint(int x, int y, bool createNewPoint);

protected:
	ObjectPtr mySelectedObject0;
	ObjectPtr mySelectedObject1;
};

#endif /* __BASEADDMODE_H__ */
