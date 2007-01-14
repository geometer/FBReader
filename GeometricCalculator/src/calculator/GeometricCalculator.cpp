#include <ZLDialogManager.h>
#include <ZLFile.h>
#include <ZLOutputStream.h>

#include "GeometricCalculator.h"
#include "GeometricCalculatorActions.h"
#include "GCOpenSceneHandler.h"
#include "GCSaveSceneHandler.h"
#include "OptionsDialog.h"
#include "SceneInfoDialog.h"
#include "ActionCode.h"

#include "../document/Document.h"
#include "../ui/DiagramView.h"
#include "../ui/AllEditModes.h"
#include "../io/SceneReader.h"
#include "../io/SceneWriter.h"
#include "../model/Scene.h"

std::string GeometricCalculator::ConfigDirectory() {
	return HomeDirectory + PathDelimiter + "." + ApplicationName();
}

const std::string GeometricCalculator::defaultSceneFileName() const {
	return ConfigDirectory() + PathDelimiter + "current.scn";
}

GeometricCalculator::GeometricCalculator(const std::string &fileName) : ZLApplication("GeometricCalculator"), myBindings("Keys") {
	myView = new DiagramView(*this, context());
	shared_ptr<Scene> savedScene = SceneReader().readScene(defaultSceneFileName());
	if (!savedScene.isNull()) {
		myView->document()->setScene(savedScene);
		if (!savedScene->isEmpty()) {
			myView->document()->setSaved(false);
		}
	}

	addAction(MODE_ADD_POINT, new SetModeAction(*myView, new AddPointMode(*myView)));
	addAction(MODE_ADD_POINT_ON_THE_LINE, new SetModeAction(*myView, new AddPointOnTheLineMode(*myView)));
	addAction(MODE_ADD_MIDDLE_POINT, new SetModeAction(*myView, new AddMiddlePointMode(*myView)));
	addAction(MODE_ADD_LINE, new SetModeAction(*myView, new AddLineMode(*myView, LINE)));
	addAction(MODE_ADD_HALFLINE, new SetModeAction(*myView, new AddLineMode(*myView, HALFLINE)));
	addAction(MODE_ADD_SEGMENT, new SetModeAction(*myView, new AddLineMode(*myView, SEGMENT)));
	addAction(MODE_ADD_PERPENDICULAR, new SetModeAction(*myView, new AddPerpendicularMode(*myView)));
	addAction(MODE_COPY_OBJECT, new SetModeAction(*myView, new CopyMode(*myView)));
	addAction(MODE_ADD_CIRCLE, new SetModeAction(*myView, new AddCircleMode(*myView)));
	addAction(MODE_ADD_RULER, new SetModeAction(*myView, new RulerMode(*myView)));
	addAction(MODE_MOVE_POINT, new SetModeAction(*myView, new MovePointMode(*myView)));
	addAction(MODE_DELETE, new SetModeAction(*myView, new EraseMode(*myView)));
	addAction(MODE_EDIT_OBJECT, new SetModeAction(*myView, new EditObjectPropertiesMode(*myView)));
	addAction(MODE_NO_ACTION, new SetModeAction(*myView, 0));
	addAction(ACTION_UNDO, new UndoAction(*myView));
	addAction(ACTION_REDO, new RedoAction(*myView));
	addAction(ACTION_SHOW_INFO, new ShowInfoDialogAction(*this));
	addAction(ACTION_SHOW_OPTIONS, new ShowOptionsDialogAction(*this));
	addAction(ACTION_NEW_SCENE, new NewSceneAction(*this));
	addAction(ACTION_OPEN_SCENE, new OpenSceneAction(*this));
	addAction(ACTION_SAVE_SCENE, new SaveSceneAction(*this));
	addAction(ACTION_REMOVE_SCENES, new RemoveScenesAction());
	addAction(ACTION_QUIT, new QuitAction(*this));
	addAction(ACTION_ZOOM_IN, new ZoomAction(*myView, 1.2));
	addAction(ACTION_ZOOM_OUT, new ZoomAction(*myView, 1 / 1.2));
	addAction(ACTION_SET_DEFAULT_ZOOM, new SetZoomAction(*myView, 100));

	shared_ptr<Toolbar::ButtonGroup> group = toolbar().createButtonGroup(MODE_NO_ACTION);
	toolbar().addButton(MODE_EDIT_OBJECT, "arrow", group);
	toolbar().addButton(MODE_ADD_POINT, "point", group);
	toolbar().addButton(MODE_ADD_POINT_ON_THE_LINE, "pointontheline", group);
	toolbar().addButton(MODE_ADD_MIDDLE_POINT, "middlepoint", group);
	toolbar().addSeparator();
	toolbar().addButton(MODE_ADD_LINE, "line", group);
	toolbar().addButton(MODE_ADD_HALFLINE, "halfline", group);
	toolbar().addButton(MODE_ADD_SEGMENT, "segment", group);
	toolbar().addSeparator();
	toolbar().addButton(MODE_ADD_PERPENDICULAR, "perpendicular", group);
	toolbar().addButton(MODE_COPY_OBJECT, "copyobject", group);
	toolbar().addButton(MODE_ADD_CIRCLE, "circle", group);
	toolbar().addSeparator();
	//toolbar().addButton(MODE_ADD_RULER, "ruler", group);
	//toolbar().addSeparator();
	toolbar().addButton(MODE_MOVE_POINT, "movepoint", group);
	toolbar().addButton(MODE_DELETE, "delete", group);
	toolbar().addSeparator();
	toolbar().addButton(ACTION_UNDO, "undo");
	toolbar().addButton(ACTION_REDO, "redo");

	menubar().addItem("New Empty Scene", ACTION_NEW_SCENE);
	menubar().addItem("Open Scene...", ACTION_OPEN_SCENE);
	menubar().addItem("Save Scene...", ACTION_SAVE_SCENE);
	menubar().addItem("Remove Scenes...", ACTION_REMOVE_SCENES);
	menubar().addSeparator();
	Menu &viewSubmenu = menubar().addSubmenu("View");
	viewSubmenu.addItem("Zoom In", ACTION_ZOOM_IN);
	viewSubmenu.addItem("100%", ACTION_SET_DEFAULT_ZOOM);
	viewSubmenu.addItem("Zoom Out", ACTION_ZOOM_OUT);
	menubar().addSeparator();
	menubar().addItem("Scene Info...", ACTION_SHOW_INFO);
	menubar().addItem("Preferences...", ACTION_SHOW_OPTIONS);
	menubar().addSeparator();
	menubar().addItem("Close", ACTION_QUIT);

	if (!fileName.empty() && saveIfChanged()) {
		open(fileName);
	}
}

void GeometricCalculator::initWindow() {
	ZLApplication::initWindow();
	setView(myView);
	trackStylus(true);
}

GeometricCalculator::~GeometricCalculator() {
	shared_ptr<ZLOutputStream> stream = ZLFile(defaultSceneFileName()).outputStream();
	if (!stream.isNull() && stream->open()) {
		const Scene &scene = *myView->document()->scene();
		SceneWriter(*stream).write(scene, scene.name());
		stream->close();
	}
	delete myView;
}

bool GeometricCalculator::saveIfChanged() {
	if (myView->document()->isSaved() || myView->document()->scene()->isEmpty()) {
		return true;
	}

	int answer = ZLDialogManager::instance().questionBox("Unsaved Scene", "Save current scene?", "&Yes", "&No", "&Cancel");

  if (answer == 0) { // answer is "Yes"
		return save();
	}
  if (answer == 1) { // answer is "No"
		return true;
	}
	return false;
}

void GeometricCalculator::newScene() {
	if (!saveIfChanged()) {
		return;
	}

	myView->document()->setScene(new Scene());
	resetWindowCaption();

	myView->repaintView();
}

void GeometricCalculator::open() {
	if (!saveIfChanged()) {
		return;
	}

	GCOpenSceneHandler handler;
	if (ZLDialogManager::instance().selectionDialog("Open Scene", handler)) {
		open(handler.fileName());
	}
}

void GeometricCalculator::open(const std::string &fileName) {
	if (!fileName.empty()) {
		myView->document()->setScene(SceneReader().readScene(fileName));
		resetWindowCaption();
		myView->repaintView();
	}
}

bool GeometricCalculator::save() {
	GCSaveSceneHandler handler(myView->document()->scene()->name());
	if (ZLDialogManager::instance().selectionDialog("Save Scene", handler)) {
		save(handler.fileName(), handler.sceneName());
		return true;
	}
	return false;
}

void GeometricCalculator::save(const std::string &fileName, const std::string &sceneName) {
	shared_ptr<ZLOutputStream> stream = ZLFile(fileName).outputStream();
	if (!stream.isNull() && stream->open()) {
		SceneWriter(*stream).write(*myView->document()->scene(), sceneName);
		stream->close();
		myView->document()->setSaved(true);
	}
}

ZLKeyBindings &GeometricCalculator::keyBindings() {
	return myBindings;
}
