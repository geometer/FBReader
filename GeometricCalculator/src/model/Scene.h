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

#ifndef __SCENE_H__
#define __SCENE_H__

#include <list>

#include "Object.h"
#include "Value.h"

class Scene {

public:
	Scene() {}
	virtual ~Scene() {}

	void setName(const std::string &name);
	const std::string &name() const;

	void setDescription(const std::string &description);
	const std::string &description() const;

	bool isEmpty() const;

	const std::list<ObjectPtr> &objects() const;
	virtual void add(const ObjectPtr object);
	virtual void remove(const ObjectPtr object);

	bool isPersistent(const ObjectPtr object) const;

	const std::list<ValuePtr> &values() const;
	virtual void add(const ValuePtr value);
	virtual void remove(const ValuePtr value);

	virtual std::list<ObjectPtr>::iterator find(const ObjectPtr object);
	virtual std::list<ObjectPtr>::const_iterator const_find(const ObjectPtr object) const;

private:
	unsigned int maxUsedPointNumber();

protected:
	std::list<ObjectPtr> myObjects;
	std::list<ValuePtr> myValues;

private:
	std::string myName;
	std::string myDescription;

private:
	Scene(const Scene&);
	Scene &operator=(const Scene&);
};

#endif /* __SCENE_H__ */
