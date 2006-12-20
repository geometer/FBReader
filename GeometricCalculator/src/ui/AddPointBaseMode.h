#ifndef __ADDPOINTBASEMODE_H__
#define __ADDPOINTBASEMODE_H__

#include "../model/PointPtr.h"
#include "BaseAddMode.h"

class AddPointBaseMode : public BaseAddMode {

protected:
	AddPointBaseMode(DiagramView &view);
	//PointPtr createCrossing(const Coordinates &coords) const;

public:
	void release();

	void onMousePressedMove(int x, int y);
	void onMouseRelease(int x, int y);

protected:
	PointPtr myPoint;
};

#endif /* __ADDPOINTBASEMODE_H__ */
