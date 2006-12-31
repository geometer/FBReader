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

ShowInfoDialogAction::ShowInfoDialogAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void ShowInfoDialogAction::run() {
	shared_ptr<ZLOptionsDialog> dialog = ZLDialogManager::instance().createOptionsDialog("SceneInfoDialog", "Scene Info");

	createInfoTab(*dialog);
	createDescriptionTab(*dialog);

	dialog->run("");

	myCalculator.resetWindowCaption();
	myCalculator.refreshWindow();
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

	Scene &scene = *myCalculator.myView->document()->scene();
	const std::list<ObjectPtr> &objects = scene.objects();

	infoTab.addOption(new SceneNameEntry(scene));
	infoTab.addOption(new ZLStringInfoEntry("Object Number", ObjectUtil::orderedClosure(objects).size()));
	infoTab.addOption(new ZLStringInfoEntry("Visible Object Number", objects.size()));
}

class DescriptionEntry : public ZLMultilineOptionEntry {

public:
	DescriptionEntry(Scene &scene);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Scene &myScene;
};

DescriptionEntry::DescriptionEntry(Scene &scene) : myScene(scene) {
}

const std::string &DescriptionEntry::name() const {
	static const std::string EMPTY;
	return EMPTY;
}

const std::string &DescriptionEntry::initialValue() const {
	return myScene.description();
}

void DescriptionEntry::onAccept(const std::string &value) {
	myScene.setDescription(value);
}

void ShowInfoDialogAction::createDescriptionTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab("Description");

	Scene &scene = *myCalculator.myView->document()->scene();

	infoTab.addOption(new DescriptionEntry(scene));
}
