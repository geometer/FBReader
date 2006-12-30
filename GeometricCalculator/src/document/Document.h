#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <vector>
#include <set>

#include <shared_ptr.h>

#include "../model/Scene.h"

class PointPtr;
class SceneDecorator;
class Command;
class CompoundCommand;

class Document {

public:
	Document(shared_ptr<Scene> scene);
	~Document();

	const std::set<ObjectPtr> &temporaryObjects();
	const std::set<ValuePtr> &temporaryValues();

	const shared_ptr<Scene> scene() const;
	void setScene(shared_ptr<Scene> scene);
	bool isSaved() const;
	void setSaved(bool saved);

	void add(ObjectPtr object);
	void addTemporary(ObjectPtr object);
	void remove(ObjectPtr object);

	void add(ValuePtr value);
	void addTemporary(ValuePtr value);
	void remove(ValuePtr value);

	void removeAllTemporaryElements();

	void movePoint(PointPtr point, int x, int y, bool undoable);

	void startTransaction();
	void endTransaction();

	bool isTemporary(const ObjectPtr object) const;
	bool isTemporary(const ValuePtr value) const;

	bool isUndoAvailable();
	void undo();
	bool isRedoAvailable();
	void redo();

private:
	void runCommand(Command *command);

private:
	shared_ptr<Scene> myScene;
	shared_ptr<Scene> myStoredScene;

	uint myDistanceToStoredScene;
	uint myUndoBufferSize;
	std::vector<const Command*> myUndoBuffer;

	bool myOriginalSceneIsReachable;
	int myDistanceToOriginalScene;

	CompoundCommand *myCompoundCommand;
	std::set<ObjectPtr> myTemporaryObjects;
	std::set<ValuePtr> myTemporaryValues;
};

#endif /* __DOCUMENT_H__ */
