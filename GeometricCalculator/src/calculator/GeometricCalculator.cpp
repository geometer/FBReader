#include <ZLDialogManager.h>
#include <ZLFile.h>
#include <ZLOutputStream.h>

#include "GCFileHandler.h"
#include "GeometricCalculator.h"
#include "GeometricCalculatorActions.h"
#include "OptionsDialog.h"
#include "SceneInfoDialog.h"
#include "ActionCode.h"

#include "../document/Document.h"
#include "../ui/DiagramView.h"
#include "../ui/AddPointMode.h"
#include "../ui/AddPointOnTheLineMode.h"
#include "../ui/AddLineMode.h"
#include "../ui/EraseMode.h"
#include "../ui/MovePointMode.h"
#include "../ui/AddPerpendicularMode.h"
#include "../ui/AddMiddlePointMode.h"
#include "../ui/AddCircleMode.h"
#include "../ui/CopyMode.h"
#include "../ui/RulerMode.h"
#include "../io/SceneReader.h"
#include "../io/SceneWriter.h"
#include "../model/Scene.h"

//#include "FileDialog.h"

const std::string GeometricCalculator::defaultSceneFileName() const {
	return HomeDirectory + PathDelimiter + "." + ApplicationName() + PathDelimiter + "current.scn";
}

GeometricCalculator::GeometricCalculator(const std::string &fileName) : ZLApplication("GeometricCalculator"), myBindings("Keys") {
	myView = new DiagramView(*this, context());
	myView->document()->setScene(SceneReader().readScene(defaultSceneFileName()));

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
	addAction(MODE_NO_ACTION, new SetModeAction(*myView, 0));
	addAction(ACTION_UNDO, new UndoAction(*myView));
	addAction(ACTION_REDO, new RedoAction(*myView));
	addAction(ACTION_SHOW_INFO, new ShowInfoDialogAction(*this));
	addAction(ACTION_SHOW_OPTIONS, new ShowOptionsDialogAction(*this));
	addAction(ACTION_NEW_SCENE, new NewSceneAction(*this));
	addAction(ACTION_OPEN_SCENE, new OpenSceneAction(*this));
	addAction(ACTION_QUIT, new QuitAction(*this));
	addAction(ACTION_ZOOM_IN, new ZoomAction(*myView, 1.2));
	addAction(ACTION_ZOOM_OUT, new ZoomAction(*myView, 1 / 1.2));

	shared_ptr<Toolbar::ButtonGroup> group = toolbar().createButtonGroup(MODE_NO_ACTION);
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

	menubar().addItem("Empty Scene", ACTION_NEW_SCENE);
	menubar().addItem("Open Scene...", ACTION_OPEN_SCENE);
	menubar().addSeparator();
	Menu &viewSubmenu = menubar().addSubmenu("View");
	viewSubmenu.addItem("Zoom In", ACTION_ZOOM_IN);
	viewSubmenu.addItem("Zoom Out", ACTION_ZOOM_OUT);
	menubar().addSeparator();
	menubar().addItem("Scene Info...", ACTION_SHOW_INFO);
	menubar().addItem("Preferences...", ACTION_SHOW_OPTIONS);
	menubar().addSeparator();
	menubar().addItem("Close", ACTION_QUIT);

	open(fileName);
}

void GeometricCalculator::initWindow() {
	ZLApplication::initWindow();
	setView(myView);
	trackStylus(true);
}

GeometricCalculator::~GeometricCalculator() {
	shared_ptr<ZLOutputStream> stream = ZLFile(defaultSceneFileName()).outputStream();
	if (!stream.isNull() && stream->open()) {
		SceneWriter(*stream).write(*myView->document()->scene());
		stream->close();
	}
	delete myView;
}

void GeometricCalculator::createMenu() {
	/*
	QPopupMenu *fileMenu = new QPopupMenu(this);
	QAccel *accelerator = new QAccel(this);
	menuBar()->insertItem("&File", fileMenu);
	accelerator->connectItem(accelerator->insertItem(Key_F), this, SLOT(showFilePopup()));

	fileMenu->insertItem("&New", this, SLOT(newScene()), Key_N);
	fileMenu->insertItem("&Open...", this, SLOT(open()), Key_O);
	fileMenu->insertItem("&Save", this, SLOT(save()), Key_S);
	fileMenu->insertItem("Save &as...", this, SLOT(saveAs()), Key_A);
	fileMenu->insertSeparator();
	fileMenu->insertItem( "E&xit", this, SLOT(close()), Key_X);

	menuBar()->insertItem("O&ptions", this, SLOT(options()));
	accelerator->connectItem(accelerator->insertItem(Key_P), this, SLOT(options()));
	menuBar()->insertItem("&Help", this, SLOT(help()));
	accelerator->connectItem(accelerator->insertItem(Key_H), this, SLOT(help()));
	*/
}

void GeometricCalculator::setMode() {
	/*
	int buttonNumber = myModeGroup->count();
	for (int i = 0; i < buttonNumber; i++) {
		QButton *selectedButton = myModeGroup->find(i);
		QButton::ToggleState state = selectedButton->state();
		if (state == 2) {
			myView->setEditMode(myModeVector[i]);
			break;
		}
	}
	*/
}

bool GeometricCalculator::saveIfChanged() {
	if (myView->document()->isSaved()) {
		return true;
	}

	/*
	int answer = QMessageBox::warning(NULL, "Unsaved scene", "Save current scene?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

  if ((answer == 0) || (answer == QMessageBox::Cancel)) {
		return false;
	}
  if (answer == QMessageBox::Yes) {
		return saveAs();
	}
	*/
	return true;
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

	GCFileHandler handler;
	ZLDialogManager::instance().openFileDialog("Open Scene", handler);
	open(handler.fileName());
}

void GeometricCalculator::open(const std::string &fileName) {
	if (!fileName.empty()) {
		myView->document()->setScene(SceneReader().readScene(fileName));
		resetWindowCaption();
		myView->repaintView();
	}

	/*
	QFile f(fileName.c_str());
	if (!f.open(IO_ReadOnly)) {
		QMessageBox::warning(NULL, "File error", "Cannot open file");
		return;
	}

	QTextStream textStream(&f);
	QTSceneReader reader(textStream);
	shared_ptr<Scene> scene = reader.read();
	myView->document()->setScene(scene);
	myView->repaintView();
	f.close();
	*/
}

bool GeometricCalculator::save(const char *fileName) {
	shared_ptr<ZLOutputStream> stream = ZLFile(fileName).outputStream();
	if (stream.isNull() || !stream->open()) {
		//QMessageBox::warning(NULL, "File error", "Cannot open file");
		return false;
	}

	SceneWriter writer(*stream);
	writer.write(*myView->document()->scene());
	myView->document()->setSaved();
	stream->close();
	return true;
}

bool GeometricCalculator::save() {
	/*
	if (myFileName == QString::null) {
		return saveAs();
	}

	return save(myFileName.ascii());
	*/
	return true;
}

bool GeometricCalculator::saveAs() {
	/*
	QString fileName = FileDialog::getSaveFileName("Save Scene As", "/mnt/card/samples", "*.scn");
	if (fileName == QString::null) {
		return false;
	}

	bool hasExtension = fileName.length() >= 4;
	if (hasExtension) {	
		QString ext = fileName;
		ext.remove(0, fileName.length() - 4);
		hasExtension = ext == ".scn";
	}
	if (!hasExtension) {
		fileName += ".scn";
	}

	QFile file(fileName);
	if (file.exists()) {
		int answer = QMessageBox::warning(NULL, "File already exists", "Overwrite file?", QMessageBox::Yes, QMessageBox::No);
		if (answer != QMessageBox::Yes) {
			return false;
		}
	}

	if (save(fileName.ascii())) {
		return true;
	} else {
		return false;
	}
	*/
	return false;
}

/*
void GeometricCalculator::closeEvent(QCloseEvent *event) {
	if (saveIfChanged()) {
		event->accept();
	} else {
		event->ignore();
	}
}
*/

void GeometricCalculator::help() {
	//QMessageBox::information(NULL, "About", "Geometric Calculator, version 0.1.0", QMessageBox::Ok);
}

void GeometricCalculator::showFilePopup() {
	//menuBar()->activateItemAt(0);
}

ZLKeyBindings &GeometricCalculator::keyBindings() {
	return myBindings;
}
