#ifndef __ERASEMODE_H__
#define __ERASEMODE_H__

#include <set>
#include "EditMode.h"

class EraseMode : public EditMode {
	private:
		ObjectPtr myObject;

	public:
		EraseMode(DiagramView &view);

		bool isAvailable() const;
		bool isObjectSelectable(ObjectPtr object) const;
		void release();

		void onMousePress(int x, int y);
		void onMouseMove(int x, int y);
};

#endif /* __ERASEMODE_H__ */
