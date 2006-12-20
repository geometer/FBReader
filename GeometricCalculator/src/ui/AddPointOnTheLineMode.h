#ifndef __ADDPOINTONTHELINEMODE_H__
#define __ADDPOINTONTHELINEMODE_H__

#include "../model/PointPtr.h"
#include "AddPointBaseMode.h"

class AddPointOnTheLineMode : public AddPointBaseMode {
public:
	AddPointOnTheLineMode(DiagramView &view);

	bool isAvailable() const;
	bool isObjectSelectable(ObjectPtr object) const;

	void onMouseMove(int x, int y);
	void onMousePress(int x, int y);
};

#endif /* __ADDPOINTONTHELINEMODE_H__ */
