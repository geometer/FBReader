#ifndef __EDITOBJECTPROPERTIESMODE_H__
#define __EDITOBJECTPROPERTIESMODE_H__

#include "EditMode.h"
#include "../model/Point.h"
#include "../model/Circle.h"

class EditObjectPropertiesMode : public EditMode {

public:
	EditObjectPropertiesMode(DiagramView &view);

	bool isAvailable() const;
	bool isObjectSelectable(ObjectPtr object) const;
	void release();

	void onMouseMove(int x, int y);
	void onMousePress(int x, int y);
	void onMouseRelease(int x, int y);

private:
	ObjectPtr mySelectedObject;
};

#endif /* __EDITOBJECTPROPERTIESMODE_H__ */
