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
	void setSceneName(const std::string &name);
	void setSceneDescription(const std::string &description);

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

	unsigned int myDistanceToStoredScene;
	unsigned int myUndoBufferSize;
	std::vector<const Command*> myUndoBuffer;

	bool myOriginalSceneIsReachable;
	int myDistanceToOriginalScene;

	CompoundCommand *myCompoundCommand;
	std::set<ObjectPtr> myTemporaryObjects;
	std::set<ValuePtr> myTemporaryValues;
};

#endif /* __DOCUMENT_H__ */
