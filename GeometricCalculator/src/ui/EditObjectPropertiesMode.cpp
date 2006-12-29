#include <ZLDialog.h>
#include <ZLDialogManager.h>
#include <ZLOptionEntry.h>

#include "EditObjectPropertiesMode.h"
#include "../document/Document.h"
#include "../model/Point.h"
#include "../model/Circle.h"

class PointNameEntry : public ZLStringOptionEntry {

public:
	PointNameEntry(Point &point);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Point &myPoint;
};

PointNameEntry::PointNameEntry(Point &point) : myPoint(point) {
}

const std::string &PointNameEntry::name() const {
	static const std::string NAME = "Name";
	return NAME;
}

const std::string &PointNameEntry::initialValue() const {
	return myPoint.name();
}

void PointNameEntry::onAccept(const std::string &value) {
	myPoint.setName(value);
}

EditObjectPropertiesMode::EditObjectPropertiesMode(DiagramView &view) : EditMode(view) {
}

void EditObjectPropertiesMode::release() {
	unselect(mySelectedObject);
	mySelectedObject.reset();
}

bool EditObjectPropertiesMode::isAvailable() const {
	return !document()->scene()->isEmpty();
}

bool EditObjectPropertiesMode::isObjectSelectable(ObjectPtr object) const {
	// TODO: replace by true
	return object->rtti() == Object::POINT;
}

void EditObjectPropertiesMode::onMousePress(int, int) {
	if (!mySelectedObject.isNull()) {
		PointPtr point = mySelectedObject;
		if (!point.isNull()) {
			shared_ptr<ZLDialog> propertiesDialog = ZLDialogManager::instance().createDialog("Point");
			propertiesDialog->addOption(new PointNameEntry(*point));
			propertiesDialog->addButton("&Ok", true);
			propertiesDialog->addButton("&Cancel", false);
			if (propertiesDialog->run()) {
				propertiesDialog->acceptValues();
				repaintView();
			}
		}
	}
}

void EditObjectPropertiesMode::onMouseRelease(int, int) {
	unselect(mySelectedObject);
	mySelectedObject.reset();
}

void EditObjectPropertiesMode::onMouseMove(int x, int y) {
	mySelectedObject = closestObject(x, y);
}
