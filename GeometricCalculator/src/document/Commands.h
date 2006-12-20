#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <set>

#include <shared_ptr.h>

#include "../model/Scene.h"
#include "../model/Point.h"
#include "../model/Line.h"

class Command {
	public:
		virtual ~Command();
		virtual void execute(shared_ptr<Scene> &scene) const = 0;
};

class CompoundCommand : public Command {
	private:
		std::list<const Command*> mySubCommands;
	public:
		~CompoundCommand();
		void add(const Command *command);
		void execute(shared_ptr<Scene> &scene) const;
};

class AddPointCommand : public Command {
	private:
		PointPtr myPoint;
		Coordinates myCoordinates;

	public:
		AddPointCommand(PointPtr point);
		void execute(shared_ptr<Scene> &scene) const;
};

class AddObjectCommand : public Command {
	private:
		ObjectPtr myObject;

	public:
		AddObjectCommand(ObjectPtr object);
		void execute(shared_ptr<Scene> &scene) const;
};

class RemoveObjectCommand : public Command {
	private:
		ObjectPtr myObject;

	public:
		RemoveObjectCommand(ObjectPtr object);
		void execute(shared_ptr<Scene> &scene) const;
};

class AddValueCommand : public Command {
	private:
		ValuePtr myValue;

	public:
		AddValueCommand(ValuePtr value);
		void execute(shared_ptr<Scene> &scene) const;
};

class RemoveValueCommand : public Command {
	private:
		ValuePtr myValue;

	public:
		RemoveValueCommand(ValuePtr value);
		void execute(shared_ptr<Scene> &scene) const;
};

class MovePointCommand : public Command {
	private:
		PointPtr myPoint;
		double myX, myY;

	public:
		MovePointCommand(PointPtr point, int x, int y);
		void execute(shared_ptr<Scene> &scene) const;
};

#endif /* __COMMANDS_H__ */
