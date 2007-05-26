/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
	shared_ptr<ZLOptionsDialog> dialog = ZLDialogManager::instance().createOptionsDialog(ZLResourceKey("sceneInfoDialog"));

	createInfoTab(*dialog);
	createDescriptionTab(*dialog);

	dialog->run();

	myCalculator.resetWindowCaption();
	myCalculator.refreshWindow();
}

class SceneNameEntry : public ZLStringOptionEntry {

public:
	SceneNameEntry(Document &document);

private:
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Document &myDocument;
};

SceneNameEntry::SceneNameEntry(Document &document) : myDocument(document) {
}

const std::string &SceneNameEntry::initialValue() const {
	return myDocument.scene()->name();
}

void SceneNameEntry::onAccept(const std::string &value) {
	myDocument.setSceneName(value);
}

void ShowInfoDialogAction::createInfoTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab(ZLResourceKey("Info"));

	Document &document = myCalculator.document();
	const std::list<ObjectPtr> &objects = document.scene()->objects();

	infoTab.addOption("Name", "", new SceneNameEntry(document));
	infoTab.addOption("Object Number", "", new ZLStringInfoEntry(ObjectUtil::orderedClosure(objects).size()));
	infoTab.addOption("Visible Object Number", "", new ZLStringInfoEntry(objects.size()));
}

class DescriptionEntry : public ZLMultilineOptionEntry {

public:
	DescriptionEntry(Document &document);

private:
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	Document &myDocument;
};

DescriptionEntry::DescriptionEntry(Document &document) : myDocument(document) {
}

const std::string &DescriptionEntry::initialValue() const {
	return myDocument.scene()->description();
}

void DescriptionEntry::onAccept(const std::string &value) {
	myDocument.setSceneDescription(value);
}

void ShowInfoDialogAction::createDescriptionTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &infoTab = dialog.createTab(ZLResourceKey("Description"));

	infoTab.addOption("", "", new DescriptionEntry(myCalculator.document()));
}
