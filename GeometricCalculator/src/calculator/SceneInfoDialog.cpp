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
	SceneNameEntry(Document &document);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Document &myDocument;
};

SceneNameEntry::SceneNameEntry(Document &document) : myDocument(document) {
}

const std::string &SceneNameEntry::name() const {
	static const std::string NAME = "Name";
	return NAME;
}

const std::string &SceneNameEntry::initialValue() const {
	return myDocument.scene()->name();
}

void SceneNameEntry::onAccept(const std::string &value) {
	myDocument.setSceneName(value);
}

void ShowInfoDialogAction::createInfoTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab("Info");

	Document &document = *myCalculator.myView->document();
	const std::list<ObjectPtr> &objects = document.scene()->objects();

	infoTab.addOption(new SceneNameEntry(document));
	infoTab.addOption(new ZLStringInfoEntry("Object Number", ObjectUtil::orderedClosure(objects).size()));
	infoTab.addOption(new ZLStringInfoEntry("Visible Object Number", objects.size()));
}

class DescriptionEntry : public ZLMultilineOptionEntry {

public:
	DescriptionEntry(Document &document);

private:
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Document &myDocument;
};

DescriptionEntry::DescriptionEntry(Document &document) : myDocument(document) {
}

const std::string &DescriptionEntry::name() const {
	static const std::string EMPTY;
	return EMPTY;
}

const std::string &DescriptionEntry::initialValue() const {
	return myDocument.scene()->description();
}

void DescriptionEntry::onAccept(const std::string &value) {
	myDocument.setSceneDescription(value);
}

void ShowInfoDialogAction::createDescriptionTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab("Description");

	infoTab.addOption(new DescriptionEntry(*myCalculator.myView->document()));
}
