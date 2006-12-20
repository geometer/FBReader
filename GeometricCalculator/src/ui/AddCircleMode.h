#ifndef __ADDCIRCLEMODE_H__
#define __ADDCIRCLEMODE_H__

#include "AddObjectMode.h"

class AddCircleMode : public AddObjectMode {
	protected:
    ObjectPtr createObject(PointPtr point0, PointPtr point1) const;

	public:
		AddCircleMode(DiagramView &view);
};

#endif /* __ADDCIRCLEMODE_H__ */
