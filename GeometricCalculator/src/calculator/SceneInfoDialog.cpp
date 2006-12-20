#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>

#include <optionEntries/ZLStringInfoEntry.h>

#include "SceneInfoDialog.h"
#include "GeometricCalculator.h"

#include "../ui/DiagramView.h"
#include "../document/Document.h"
#include "../model/Scene.h"
#include "../model/ObjectUtil.h"

ShowInfoDialogAction::ShowInfoDialogAction(GeometricCalculator &window) : myWindow(window) {
}

void ShowInfoDialogAction::run() {
	ZLOptionsDialog *dialog = ZLDialogManager::instance().createOptionsDialog("SceneInfoDialog", "Scene Info");

	createInfoTab(*dialog);

	dialog->run("");
	delete dialog;

	myWindow.resetWindowCaption();
	myWindow.refreshWindow();
}

class SceneNameEntry : public ZLStringOptionEntry {

public:
	SceneNameEntry(Scene &scene);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Scene &myScene;
};

SceneNameEntry::SceneNameEntry(Scene &scene) : myScene(scene) {
}

const std::string &SceneNameEntry::name() const {
	static const std::string NAME = "Name";
	return NAME;
}

const std::string &SceneNameEntry::initialValue() const {
	return myScene.name();
}

void SceneNameEntry::onAccept(const std::string &value) {
	myScene.setName(value);
}

void ShowInfoDialogAction::createInfoTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab("Info");

	Scene &scene = *myWindow.myView->document()->scene();
	const std::list<ObjectPtr> &objects = scene.objects();

	infoTab.addOption(new SceneNameEntry(scene));
	infoTab.addOption(new ZLStringInfoEntry("Object Number", ObjectUtil::orderedClosure(objects).size()));
	infoTab.addOption(new ZLStringInfoEntry("Visible Object Number", objects.size()));
}
