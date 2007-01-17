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

#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Line.h"
#include "Commands.h"

Command::~Command() {
}

CompoundCommand::~CompoundCommand() {
	for (std::list<const Command*>::iterator it = mySubCommands.begin(); it != mySubCommands.end(); it++) {
		delete *it;
	}
	mySubCommands.clear();
}

void CompoundCommand::add(const Command *command) {
	mySubCommands.push_back(command);
}

void CompoundCommand::execute(shared_ptr<Scene> &scene) const {
	for (std::list<const Command*>::const_iterator it = mySubCommands.begin(); it != mySubCommands.end(); it++) {
		(*it)->execute(scene);
	}
}

AddPointCommand::AddPointCommand(PointPtr point) : myPoint(point), myCoordinates(point->coordinates()) {
}

void AddPointCommand::execute(shared_ptr<Scene> &scene) const {
	scene->add(myPoint);
	std::list<ObjectPtr>::iterator it = scene->find(myPoint);
	PointPtr(*it)->moveTo(myCoordinates);
}

AddObjectCommand::AddObjectCommand(ObjectPtr object) : myObject(object) {
}

void AddObjectCommand::execute(shared_ptr<Scene> &scene) const {
	scene->add(myObject);
}

RemoveObjectCommand::RemoveObjectCommand(ObjectPtr object) : myObject(object) {
}

void RemoveObjectCommand::execute(shared_ptr<Scene> &scene) const {
	scene->remove(myObject);
}

AddValueCommand::AddValueCommand(ValuePtr value) : myValue(value) {
}

void AddValueCommand::execute(shared_ptr<Scene> &scene) const {
	scene->add(myValue);
}

RemoveValueCommand::RemoveValueCommand(ValuePtr value) : myValue(value) {
}

void RemoveValueCommand::execute(shared_ptr<Scene> &scene) const {
	scene->remove(myValue);
}

MovePointCommand::MovePointCommand(PointPtr point, int x, int y) : myPoint(point), myX(x), myY(y) {
}

void MovePointCommand::execute(shared_ptr<Scene> &scene) const {
	std::list<ObjectPtr>::const_iterator it = scene->find(myPoint);
	if (it != scene->objects().end()) {
		PointPtr(*it)->moveTo(myX, myY);
	}
}
