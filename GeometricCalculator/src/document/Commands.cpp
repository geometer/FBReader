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
