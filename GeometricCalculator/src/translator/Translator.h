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

#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <map>

#include "../model/Object.h"
#include "../polynom/Polynom.h"

class Translator {
	private:
		std::map<ObjectPtr,int> myObjects;

		Monom monom(int objectNumber, const char *name);
		
	public:
		Translator(const std::list<ObjectPtr> &objects);
		Monom monom(const Object &object, const char *name);
		Monom monom(ObjectPtr object, const char *name);
		std::list<Polynom> translate();
};

#endif /* __TRANSLATOR_H__ */
