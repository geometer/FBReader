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

	dialog->run("");

	myCalculator.refreshWindow();
}

void ShowOptionsDialogAction::createViewTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &viewTab = dialog.createTab("View");

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	viewTab.addOption(new ZLSimpleSpinOptionEntry("Zoom, %", view.ZoomOption, 10));
}

void ShowOptionsDialogAction::createDrawingTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &drawingTab = dialog.createTab("Drawing");

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	drawingTab.addOption(new ZLSimpleBooleanOptionEntry("Connect New Lines To Existing Points Only", view.ExistingPointsOnlyOption));
	drawingTab.addOption(new ZLSimpleBooleanOptionEntry("Draw Segment For Middle Point", view.CreateSegmentForMiddlePointOption));
}

void ShowOptionsDialogAction::createPointTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &pointTab = dialog.createTab("Points");

	DiagramView &view = (DiagramView&)*myCalculator.currentView();
	pointTab.addOption(new ZLSimpleSpinOptionEntry("Point Radius", view.PointRadiusOption, 1));

	ZLToggleBooleanOptionEntry *showNamesEntry =
		new ZLToggleBooleanOptionEntry("Show Point Names", view.ShowPointNamesOption);
	pointTab.addOption(showNamesEntry);

	ZLOptionEntry *fontFamilyEntry =
		new ZLFontFamilyOptionEntry("Font Family", view.PointFontFamilyOption, view.context());
	showNamesEntry->addDependentEntry(fontFamilyEntry);
	pointTab.addOption(fontFamilyEntry);

	ZLOptionEntry *fontSizeEntry =
		new ZLSimpleSpinOptionEntry("Font Size", view.PointFontSizeOption, 2);
	showNamesEntry->addDependentEntry(fontSizeEntry);
	pointTab.addOption(fontSizeEntry);

	showNamesEntry->onStateChanged(showNamesEntry->initialState());
}

void ShowOptionsDialogAction::createColorsTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &colorsTab = dialog.createTab("Colors");

	DiagramView &view = (DiagramView&)*myCalculator.currentView();

	ZLColorOptionBuilder builder;
	static const std::string BACKGROUND = "Background";
	builder.addOption(BACKGROUND, view.BackgroundColorOption);
	builder.addOption("Active Object", view.ActiveColorOption);
	builder.addOption("Inactive Object", view.InactiveColorOption);
	builder.addOption("Selected Object", view.SelectedColorOption);
	builder.addOption("Ruler", view.RulerColorOption);
	builder.setInitial(BACKGROUND);
	colorsTab.addOption(builder.comboEntry());
	colorsTab.addOption(builder.colorEntry());
}

class KeyOptionEntry : public ZLSimpleKeyOptionEntry {

public:
	KeyOptionEntry(ZLKeyBindings &bindings);
	const CodeIndexBimap &codeIndexBimap() const;

private:
	void addAction(int code, const std::string &name);

private:
	CodeIndexBimap myBimap;
};

KeyOptionEntry::KeyOptionEntry(ZLKeyBindings &bindings) : ZLSimpleKeyOptionEntry(bindings) {
	addAction(NO_ACTION, "None");

	addAction(ACTION_NEW_SCENE, "New Empty Scene");
	addAction(ACTION_OPEN_SCENE, "Open Scene");
	addAction(ACTION_SAVE_SCENE, "Save Scene");
	//addAction(MODE_ADD_POINT, "");
	//addAction(MODE_ADD_POINT_ON_THE_LINE, "");
	//addAction(MODE_ADD_MIDDLE_POINT, "");
	//addAction(MODE_ADD_LINE, "");
	//addAction(MODE_ADD_HALFLINE, "");
	//addAction(MODE_ADD_SEGMENT, "");
	//addAction(MODE_ADD_PERPENDICULAR, "");
	//addAction(MODE_COPY_OBJECT, "");
	//addAction(MODE_ADD_CIRCLE, "");
	//addAction(MODE_ADD_RULER, "");
	//addAction(MODE_MOVE_POINT, "");
	//addAction(MODE_DELETE, "");
	//addAction(MODE_EDIT_OBJECT, "");
	addAction(ACTION_SHOW_INFO, "Show Scene Info Dialog");
	addAction(ACTION_SHOW_OPTIONS, "Show Options Dialog");
	addAction(ACTION_UNDO, "Undo");
	addAction(ACTION_REDO, "Redo");
	addAction(ACTION_ZOOM_IN, "Zoom In");
	addAction(ACTION_ZOOM_OUT, "Zoom Out");
	addAction(ACTION_SET_DEFAULT_ZOOM, "Zoom 100%");
	addAction(ACTION_QUIT, "Quit");
}

const ZLSimpleKeyOptionEntry::CodeIndexBimap &KeyOptionEntry::codeIndexBimap() const {
	return myBimap;
}

void KeyOptionEntry::addAction(int code, const std::string &name) {
	myBimap.insert(code);
	addActionName(name);
}

void ShowOptionsDialogAction::createKeysTab(ZLOptionsDialog &dialog) {
	ZLDialogContent &keysTab = dialog.createTab("Keys");
	keysTab.addOption(new KeyOptionEntry(myCalculator.keyBindings()));
}
