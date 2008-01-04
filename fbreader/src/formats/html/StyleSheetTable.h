/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __STYLESHEETTABLE_H__
#define __STYLESHEETTABLE_H__

#include <string>
#include <map>
#include <vector>

class StyleSheetTable {

public:
	struct Key {
		std::string TagName;
		std::string ClassName;

		bool operator < (const Key &key) const;
	};

	typedef std::map<std::string,std::vector<std::string> > AttributeMap;

	typedef std::pair<Key,AttributeMap> Element;

public:
	void addElement(const Element &element);

	void dump() const;

private:
	std::map<Key,AttributeMap> myMap;
};

inline bool StyleSheetTable::Key::operator < (const StyleSheetTable::Key &key) const {
	return (TagName < key.TagName) || ((TagName == key.TagName) && (ClassName < key.ClassName));
}

#endif /* __STYLESHEETTABLE_H__ */
