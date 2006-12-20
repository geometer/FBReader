#ifndef __COPYMODE_H__
#define __COPYMODE_H__

#include "AddConstructMode.h"
#include "../model/Point.h"
#include "../model/Line.h"

class CopyMode : public AddConstructMode {
	public:
		CopyMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;

		void onMousePress(int x, int y);
		void onMouseRelease(int x, int y);

	private:
		bool mySamePointOrder;
};

#endif /* __COPYMODE_H__ */
