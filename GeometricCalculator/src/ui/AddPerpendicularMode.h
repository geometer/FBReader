#ifndef __ADDPERPENDICULARMODE_H__
#define __ADDPERPENDICULARMODE_H__

#include "AddConstructMode.h"
#include "../model/Point.h"
#include "../model/Line.h"

class AddPerpendicularMode : public AddConstructMode {
	private:
		PointPtr myCrossingPoint;

	protected:
		void selectNewObjects();
		void unselectNewObjects();

	public:
		AddPerpendicularMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMousePress(int x, int y);
		void onMouseRelease(int x, int y);
};

#endif /* __ADDPERPENDICULARMODE_H__ */
