/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESGENRE_H__
#define __LITRESGENRE_H__

#include <string>
#include <vector>

#include <shared_ptr.h>

struct LitResGenre {
	std::string Id;
	std::string Title;
	std::vector<shared_ptr<LitResGenre> > Children;

	LitResGenre();
	LitResGenre(const std::string &id, const std::string &title);
};

inline LitResGenre::LitResGenre() {}
inline LitResGenre::LitResGenre(const std::string &id, const std::string &title) : Id(id), Title(title) {}

#endif /* __LITRESGENRE_H__ */
