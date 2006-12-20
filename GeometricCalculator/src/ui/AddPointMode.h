#ifndef __ADDPOINTMODE_H__
#define __ADDPOINTMODE_H__

#include "../model/PointPtr.h"
#include "AddPointBaseMode.h"

class AddPointMode : public AddPointBaseMode {
	public:
		AddPointMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;

		void onMouseMove(int x, int y);
		void onMousePress(int x, int y);
};

#endif /* __ADDPOINTMODE_H__ */
