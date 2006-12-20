#ifndef __ADDOBJECTMODE_H__
#define __ADDOBJECTMODE_H__

#include "BaseAddMode.h"
#include "../model/Point.h"

class AddObjectMode : public BaseAddMode {
	protected:
		AddObjectMode(DiagramView &view);
    virtual ObjectPtr createObject(PointPtr point0, PointPtr point1) const = 0;
		
	public:

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMousePress(int x, int y);
		void onMouseRelease(int x, int y);
		void onMouseMove(int x, int y);
		void onMousePressedMove(int x, int y);

	private:
		PointPtr mySelectedPoint;
		PointPtr myMousePoint;
		ObjectPtr myNewObject;
};

#endif /* __ADDOBJECTMODE_H__ */
