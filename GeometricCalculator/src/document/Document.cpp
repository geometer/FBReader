#include "Document.h"
#include "Commands.h"
#include "../modelDecorator/Decorator.h"
#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Circle.h"

Document::Document(shared_ptr<Scene> scene) {
	myUndoBufferSize = 100;
	setScene(scene);
	myCompoundCommand = NULL;
}

Document::~Document() {
	for (std::vector<const Command*>::iterator it = myUndoBuffer.begin(); it != myUndoBuffer.end(); it++) {
		delete *it;
	}
}

const std::set<ObjectPtr> &Document::temporaryObjects() {
	return myTemporaryObjects;
}

const std::set<ValuePtr> &Document::temporaryValues() {
	return myTemporaryValues;
}

void Document::setScene(shared_ptr<Scene> scene) {
	myScene = scene;
	myStoredScene = new SceneDecorator(myScene);

	for (std::vector<const Command*>::iterator it = myUndoBuffer.begin(); it != myUndoBuffer.end(); it++) {
		delete *it;
	}
	myUndoBuffer.clear();
	myDistanceToStoredScene = 0;

	setSaved();
}

const shared_ptr<Scene> Document::scene() const {
	return myScene;
}

bool Document::isSaved() const {
	return myOriginalSceneIsReachable && (myDistanceToOriginalScene == 0);
}

void Document::setSaved() {
	myOriginalSceneIsReachable = true;
	myDistanceToOriginalScene = 0;
}

bool Document::isTemporary(const ObjectPtr object) const {
	return myTemporaryObjects.find(object) != myTemporaryObjects.end();
}

bool Document::isTemporary(const ValuePtr value) const {
	return myTemporaryValues.find(value) != myTemporaryValues.end();
}

void Document::movePoint(PointPtr point, int x, int y, bool undoable) {
	if (undoable) {
		runCommand(new MovePointCommand(point, x, y));
	} else {
		point->moveTo(x, y);
	}
}

void Document::add(ObjectPtr object) {
	if (!object.isNull()) {
		PointPtr point = object;
		if (!point.isNull()) {
			runCommand(new AddPointCommand(point));
		} else {
			runCommand(new AddObjectCommand(object));
		}
	}
}

void Document::addTemporary(ObjectPtr object) {
	if (!object.isNull()) {
		myTemporaryObjects.insert(object);
	}
}

void Document::remove(ObjectPtr object) {
	if (!object.isNull()) {
		if (isTemporary(object)) {
			myTemporaryObjects.erase(object);
		} else {
			runCommand(new RemoveObjectCommand(object));
		}
	}
}

void Document::add(ValuePtr value) {
	runCommand(new AddValueCommand(value));
}

void Document::addTemporary(ValuePtr value) {
	myTemporaryValues.insert(value);
}

void Document::remove(ValuePtr value) {
	if (!value.isNull()) {
		if (isTemporary(value)) {
			myTemporaryValues.erase(value);
		} else {
			runCommand(new RemoveValueCommand(value));
		}
	}
}

void Document::removeAllTemporaryElements() {
	myTemporaryObjects.clear();
	myTemporaryValues.clear();
}

void Document::startTransaction() {
	myCompoundCommand = new CompoundCommand();
}

void Document::endTransaction() {
	Command *compoundCommand = myCompoundCommand;
	myCompoundCommand = NULL;
	runCommand(compoundCommand);
}

void Document::runCommand(Command *command) {
	if (myCompoundCommand != NULL) {
		myCompoundCommand->add(command);
		return;
	}

	command->execute(myScene);

	if (myDistanceToOriginalScene < 0) {
		myOriginalSceneIsReachable = false;
	}
	myDistanceToOriginalScene++;

	while (myUndoBuffer.size() > myDistanceToStoredScene) {
    delete *(myUndoBuffer.end() - 1);
    myUndoBuffer.erase(myUndoBuffer.end() - 1);
	}

	myUndoBuffer.push_back(command);
	myDistanceToStoredScene++;
	
	while (myDistanceToStoredScene > myUndoBufferSize) {
		const Command *extraCommand = myUndoBuffer.front();
    extraCommand->execute(myStoredScene);
		myUndoBuffer.erase(myUndoBuffer.begin());
		delete extraCommand;
		myDistanceToStoredScene--;
	}
}

bool Document::isUndoAvailable() {
	return myDistanceToStoredScene > 0;
}

bool Document::isRedoAvailable() {
	return myUndoBuffer.size() > myDistanceToStoredScene;
}

void Document::undo() {
	if (isUndoAvailable()) {
		const std::string name = myScene->name();
		myScene = ((SceneDecorator*)&*myStoredScene)->restore();
		myScene->setName(name);
		myDistanceToStoredScene--;
		myDistanceToOriginalScene--;
		std::vector<const Command*>::const_iterator it = myUndoBuffer.begin();
		for (uint i = 0; i < myDistanceToStoredScene; i++, it++) {
			(*it)->execute(myScene);
		}
	}
}

void Document::redo() {
	if (isRedoAvailable()) {
		myUndoBuffer[myDistanceToStoredScene]->execute(myScene);
		myDistanceToStoredScene++;
		myDistanceToOriginalScene++;
	}
}
