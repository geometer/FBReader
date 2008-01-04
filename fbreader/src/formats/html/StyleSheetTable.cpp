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

#include "StyleSheetTable.h"

void StyleSheetTable::addElement(const Element &element) {
	if ((!element.first.TagName.empty() || !element.first.ClassName.empty()) &&
			!element.second.empty()) {
		myMap.insert(element);
	}
}

//#include <iostream>

void StyleSheetTable::dump() const {
/*
	for (std::map<Key,AttributeMap>::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
		std::cerr << it->first.TagName << " . " << it->first.ClassName << "\n";
		for (AttributeMap::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt) {
			std::cerr << "  " << jt->first << " =";
			for (std::vector<std::string>::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt) {
				std::cerr << " " << *kt;
			}
			std::cerr << "\n";
		}
	}
*/
}
