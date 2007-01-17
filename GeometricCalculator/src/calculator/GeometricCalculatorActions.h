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

#include <ZLApplication.h>

class GeometricCalculator;
class DiagramView;
class EditMode;

class QuitAction : public ZLApplication::Action {

public:
	QuitAction(GeometricCalculator &calculator);
	void run();

private:
	GeometricCalculator &myCalculator;
};

class ZoomAction : public ZLApplication::Action {

public:
	ZoomAction(DiagramView &view, double coefficient);
	void run();
	bool isEnabled();

private:
	DiagramView &myView;
	const double myCoefficient;
};

class SetZoomAction : public ZLApplication::Action {

public:
	SetZoomAction(DiagramView &view, int value);
	void run();
	bool isEnabled();

private:
	DiagramView &myView;
	const int myValue;
};

class UndoAction : public ZLApplication::Action {

public:
	UndoAction(DiagramView &view);
	bool isEnabled();
	void run();

private:
	DiagramView &myView;
};

class RedoAction : public ZLApplication::Action {

public:
	RedoAction(DiagramView &view);
	bool isEnabled();
	void run();

private:
	DiagramView &myView;
};

class SetModeAction : public ZLApplication::Action {

public:
	SetModeAction(DiagramView &view, shared_ptr<EditMode> editMode);
	bool isEnabled();
	void run();

private:
	DiagramView &myView;
	shared_ptr<EditMode> myEditMode;
};

class OpenSceneAction : public ZLApplication::Action {

public:
	OpenSceneAction(GeometricCalculator &calculator);
	void run();

private:
	GeometricCalculator &myCalculator;
};

class SaveSceneAction : public ZLApplication::Action {

public:
	SaveSceneAction(GeometricCalculator &calculator);
	void run();

private:
	GeometricCalculator &myCalculator;
};

class NewSceneAction : public ZLApplication::Action {

public:
	NewSceneAction(GeometricCalculator &calculator);
	bool isEnabled();
	void run();

private:
	GeometricCalculator &myCalculator;
};

class RemoveScenesAction : public ZLApplication::Action {

public:
	RemoveScenesAction();
	void run();
};
