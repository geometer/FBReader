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
