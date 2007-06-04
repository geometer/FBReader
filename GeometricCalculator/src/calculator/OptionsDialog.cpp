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

#include <optionEntries/ZLFontFamilyOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>
#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLSimpleKeyOptionEntry.h>

#include "OptionsDialog.h"
#include "GeometricCalculator.h"
#include "ActionCode.h"

#include "../ui/DiagramView.h"

ShowOptionsDialogAction::ShowOptionsDialogAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void ShowOptionsDialogAction::run() {
	shared_ptr<ZLOptionsDialog> dialog = ZLDialogManager::instance().createOptionsDialog(ZLResourceKey("optionsDialog"));

	createViewTab(*dialog);
	createDrawingTab(*dialog);
	createPointTab(*dialog);
	createColorsTab(*dialog);
	createKeysTab(*dialog);

	dialog->run();

	myCalculator.refreshWindow();
}

void ShowOptionsDialogAction::createViewTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &viewTab = dialog.createTab(ZLResourceKey("view"));

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	viewTab.addOption(ZLResourceKey("zoom"), new ZLSimpleSpinOptionEntry(view.ZoomOption, 10));
}

void ShowOptionsDialogAction::createDrawingTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &drawingTab = dialog.createTab(ZLResourceKey("drawing"));

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	drawingTab.addOption(ZLResourceKey("connectPointsOnly"), view.ExistingPointsOnlyOption);
	drawingTab.addOption(ZLResourceKey("drawSegmentForMiddlePoint"), view.CreateSegmentForMiddlePointOption);
}

void ShowOptionsDialogAction::createPointTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &pointTab = dialog.createTab(ZLResourceKey("points"));

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	pointTab.addOption(ZLResourceKey("radius"), new ZLSimpleSpinOptionEntry(view.PointRadiusOption, 1));

	ZLToggleBooleanOptionEntry *showNamesEntry =
		new ZLToggleBooleanOptionEntry(view.ShowPointNamesOption);
	pointTab.addOption(ZLResourceKey("showNames"), showNamesEntry);

	ZLOptionEntry *fontFamilyEntry =
		new ZLFontFamilyOptionEntry(view.PointFontFamilyOption, view.context());
	showNamesEntry->addDependentEntry(fontFamilyEntry);
	pointTab.addOption(ZLResourceKey("fontFamily"), fontFamilyEntry);

	ZLOptionEntry *fontSizeEntry =
		new ZLSimpleSpinOptionEntry(view.PointFontSizeOption, 2);
	showNamesEntry->addDependentEntry(fontSizeEntry);
	pointTab.addOption(ZLResourceKey("fontSize"), fontSizeEntry);

	showNamesEntry->onStateChanged(showNamesEntry->initialState());
}

void ShowOptionsDialogAction::createColorsTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &colorsTab = dialog.createTab(ZLResourceKey("colors"));

	DiagramView &view = (DiagramView&)*myCalculator.currentView();

	ZLResourceKey colorKey("colorFor");
	const ZLResource &resource = colorsTab.resource(colorKey);
	ZLColorOptionBuilder builder;
	const std::string BACKGROUND = resource["background"].value();
	builder.addOption(BACKGROUND, view.BackgroundColorOption);
	builder.addOption(resource["activeObject"].value(), view.ActiveColorOption);
	builder.addOption(resource["inactiveObject"].value(), view.InactiveColorOption);
	builder.addOption(resource["selectedObject"].value(), view.SelectedColorOption);
	//builder.addOption(resource["ruler"], view.RulerColorOption);
	builder.setInitial(BACKGROUND);
	colorsTab.addOption(colorKey, builder.comboEntry());
	colorsTab.addOption("", "", builder.colorEntry());
}

class KeyOptionEntry : public ZLSimpleKeyOptionEntry {

public:
	KeyOptionEntry(ZLKeyBindings &bindings, const ZLResource &resource);
	const CodeIndexBimap &codeIndexBimap() const;

private:
	void addAction(int code, const ZLResourceKey &key);

private:
	const ZLResource &myResource;
	CodeIndexBimap myBimap;
};

KeyOptionEntry::KeyOptionEntry(ZLKeyBindings &bindings, const ZLResource &resource) : ZLSimpleKeyOptionEntry(bindings), myResource(resource) {
	addAction(NO_ACTION, ZLResourceKey("none"));

	addAction(ACTION_NEW_SCENE, ZLResourceKey("newScene"));
	addAction(ACTION_OPEN_SCENE, ZLResourceKey("open"));
	addAction(ACTION_SAVE_SCENE, ZLResourceKey("save"));
	//addAction(MODE_ADD_POINT, ZLResourceKey(""));
	//addAction(MODE_ADD_POINT_ON_THE_LINE, ZLResourceKey(""));
	//addAction(MODE_ADD_MIDDLE_POINT, ZLResourceKey(""));
	//addAction(MODE_ADD_LINE, ZLResourceKey(""));
	//addAction(MODE_ADD_HALFLINE, ZLResourceKey(""));
	//addAction(MODE_ADD_SEGMENT, ZLResourceKey(""));
	//addAction(MODE_ADD_PERPENDICULAR, ZLResourceKey(""));
	//addAction(MODE_COPY_OBJECT, ZLResourceKey(""));
	//addAction(MODE_ADD_CIRCLE, ZLResourceKey(""));
	//addAction(MODE_ADD_RULER, ZLResourceKey(""));
	//addAction(MODE_MOVE_POINT, ZLResourceKey(""));
	//addAction(MODE_DELETE, ZLResourceKey(""));
	//addAction(MODE_EDIT_OBJECT, ZLResourceKey(""));
	addAction(ACTION_SHOW_INFO, ZLResourceKey("showInfoDialog"));
	addAction(ACTION_SHOW_OPTIONS, ZLResourceKey("showOptionsDialog"));
	addAction(ACTION_UNDO, ZLResourceKey("undo"));
	addAction(ACTION_REDO, ZLResourceKey("redo"));
	addAction(ACTION_ZOOM_IN, ZLResourceKey("zoomIn"));
	addAction(ACTION_ZOOM_OUT, ZLResourceKey("zoomOut"));
	addAction(ACTION_SET_DEFAULT_ZOOM, ZLResourceKey("zoomNormal"));
	addAction(ACTION_QUIT, ZLResourceKey("quit"));
}

const ZLSimpleKeyOptionEntry::CodeIndexBimap &KeyOptionEntry::codeIndexBimap() const {
	return myBimap;
}

void KeyOptionEntry::addAction(int code, const ZLResourceKey &key) {
	myBimap.insert(code);
	addActionName(myResource[key].value());
}

void ShowOptionsDialogAction::createKeysTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &keysTab = dialog.createTab(ZLResourceKey("keys"));
	keysTab.addOption("", "", new KeyOptionEntry(myCalculator.keyBindings(), keysTab.resource(ZLResourceKey("action"))));
}
