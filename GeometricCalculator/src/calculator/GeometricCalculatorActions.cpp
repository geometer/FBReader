#include <ZLDialogManager.h>

#include "GeometricCalculatorActions.h"
#include "GeometricCalculator.h"
#include "GCRemoveSceneHandler.h"

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

SetModeAction::SetModeAction(DiagramView &view, shared_ptr<EditMode> editMode) : myView(view), myEditMode(editMode) {
}

bool SetModeAction::isEnabled() {
	return myEditMode.isNull() || myEditMode->isAvailable();
}

void SetModeAction::run() {
	myView.setEditMode(myEditMode);
}

UndoAction::UndoAction(DiagramView &view) : myView(view) {
}

bool UndoAction::isEnabled() {
	return myView.document()->isUndoAvailable();
}

void UndoAction::run() {
	myView.document()->undo();
	myView.repaintView();
}

RedoAction::RedoAction(DiagramView &view) : myView(view) {
}

bool RedoAction::isEnabled() {
	return myView.document()->isRedoAvailable();
}

void RedoAction::run() {
	myView.document()->redo();
	myView.repaintView();
}

NewSceneAction::NewSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

bool NewSceneAction::isEnabled() {
	return !myCalculator.myView->document()->scene()->isEmpty();
}

void NewSceneAction::run() {
	myCalculator.newScene();
}

OpenSceneAction::OpenSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void OpenSceneAction::run() {
	myCalculator.open();
}

SaveSceneAction::SaveSceneAction(GeometricCalculator &calculator) : myCalculator(calculator) {
}

void SaveSceneAction::run() {
	myCalculator.save();
}

RemoveScenesAction::RemoveScenesAction() {
}

void RemoveScenesAction::run() {
	GCRemoveSceneHandler handler;
	ZLDialogManager::instance().selectionDialog("Remove Scenes", handler);
}
