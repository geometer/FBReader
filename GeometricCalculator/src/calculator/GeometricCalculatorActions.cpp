#include "GeometricCalculatorActions.h"
#include "GeometricCalculator.h"

#include "../document/Document.h"
#include "../ui/DiagramView.h"
#include "../ui/EditMode.h"

QuitAction::QuitAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void QuitAction::run() {
	myCalculator.quit();
}

ZoomAction::ZoomAction(DiagramView &view, double coefficient) : myView(view), myCoefficient(coefficient) {
}

void ZoomAction::run() {
	myView.ZoomOption.setValue((int)(myCoefficient * myView.ZoomOption.value() + .5));
	myView.repaintView();
}

bool ZoomAction::isEnabled() {
	if (myCoefficient < 1) {
		return myView.ZoomOption.value() != myView.ZoomOption.minValue();
	} else {
		return myView.ZoomOption.value() != myView.ZoomOption.maxValue();
	}
}

GeometricCalculator::SetModeAction::SetModeAction(DiagramView &view, shared_ptr<EditMode> editMode) : myView(view), myEditMode(editMode) {
}

bool GeometricCalculator::SetModeAction::isEnabled() {
	return myEditMode.isNull() || myEditMode->isAvailable();
}

void GeometricCalculator::SetModeAction::run() {
	myView.setEditMode(myEditMode);
}

GeometricCalculator::UndoAction::UndoAction(DiagramView &view) : myView(view) {
}

bool GeometricCalculator::UndoAction::isEnabled() {
	return myView.document()->isUndoAvailable();
}

void GeometricCalculator::UndoAction::run() {
	myView.document()->undo();
	myView.repaintView();
}

GeometricCalculator::RedoAction::RedoAction(DiagramView &view) : myView(view) {
}

bool GeometricCalculator::RedoAction::isEnabled() {
	return myView.document()->isRedoAvailable();
}

void GeometricCalculator::RedoAction::run() {
	myView.document()->redo();
	myView.repaintView();
}

GeometricCalculator::NewSceneAction::NewSceneAction(GeometricCalculator &window) : myWindow(window) {
}

bool GeometricCalculator::NewSceneAction::isEnabled() {
	return !myWindow.myView->document()->scene()->isEmpty();
}

void GeometricCalculator::NewSceneAction::run() {
	myWindow.newScene();
}

GeometricCalculator::OpenSceneAction::OpenSceneAction(GeometricCalculator &window) : myWindow(window) {
}

void GeometricCalculator::OpenSceneAction::run() {
	myWindow.open();
}
