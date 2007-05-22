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

SetZoomAction::SetZoomAction(DiagramView &view, int value) : myView(view), myValue(value) {
}

void SetZoomAction::run() {
	myView.ZoomOption.setValue(myValue);
	myView.repaintView();
}

bool SetZoomAction::isEnabled() {
	return myView.ZoomOption.value() != myValue;
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
	return !myCalculator.document().scene()->isEmpty();
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
	ZLDialogManager::instance().selectionDialog(ZLResourceKey("removeScenesDialog"), handler);
}
