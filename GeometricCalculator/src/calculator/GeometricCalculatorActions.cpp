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

GeometricCalculator::NewSceneAction::NewSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

bool GeometricCalculator::NewSceneAction::isEnabled() {
	return !myCalculator.myView->document()->scene()->isEmpty();
}

void GeometricCalculator::NewSceneAction::run() {
	myCalculator.newScene();
}

GeometricCalculator::OpenSceneAction::OpenSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void GeometricCalculator::OpenSceneAction::run() {
	myCalculator.open();
}

GeometricCalculator::SaveSceneAction::SaveSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void GeometricCalculator::SaveSceneAction::run() {
	myCalculator.save();
}

bool GeometricCalculator::SaveSceneAction::isEnabled() {
	// TODO: implement or remove
	return true;
}
