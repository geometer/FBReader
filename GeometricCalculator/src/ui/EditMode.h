#ifndef __EDITMODE_H__
#define __EDITMODE_H__

#include "../model/Scene.h"
#include "DiagramView.h"

class Document;
class LinePtr;

class EditMode {

public:
	EditMode(DiagramView &view);
	virtual ~EditMode();

	virtual bool isAvailable() const = 0;
	virtual bool isObjectSelectable(ObjectPtr object) const = 0;
	virtual void release() = 0;

	virtual void onMousePress(int x, int y);
	virtual void onMouseRelease(int x, int y);
	virtual void onMouseMove(int x, int y);
	virtual void onMousePressedMove(int x, int y);

private:
	double distanceToLine(LinePtr line, double x, double y);

protected:
	Document *document() const;

	ObjectPtr closestObject(double x, double y);
	ObjectPtr closestObject(const Coordinates coords);
	bool isSelected(ObjectPtr object) const;
	void select(ObjectPtr object);
	void unselect(ObjectPtr object);

	const DiagramView &view() const;

	void repaintView();

private:
	DiagramView &myView;
};

inline EditMode::EditMode(DiagramView &view) : myView(view) {}

inline bool EditMode::isSelected(ObjectPtr object) const { return myView.isSelected(object); }
inline void EditMode::select(ObjectPtr object) { myView.select(object); }
inline void EditMode::unselect(ObjectPtr object) { myView.unselect(object); }

#endif /* __EDITMODE_H__ */
