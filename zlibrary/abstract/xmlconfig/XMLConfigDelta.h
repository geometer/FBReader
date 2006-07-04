/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __XMLCONFIGDELTA_H__
#define __XMLCONFIGDELTA_H__

#include <map>
#include <set>
#include <string>

#include "XMLConfigValue.h"

class XMLConfigDeltaGroup {
  XMLConfigDeltaGroup(std::set<std::string> &categories) : myCategories(categories) {}
  void setValue(const std::string &name, const std::string &value, const std::string &category);
  void unsetValue(const std::string &name);
  void setCategory(const std::string &name, const std::string &category);

private:
  std::map<std::string,XMLConfigValue> myValues;
  std::set<std::string> myRemovedNames;
  std::set<std::string> &myCategories;
};

class XMLConfigDelta {

public:
  XMLConfigDelta(std::set<std::string> &categories) : myCategories(categories) {}
	~XMLConfigDelta();
  XMLConfigDeltaGroup *getGroup(const std::string &name);
	void clear();

	void setValue(const std::string &group, const std::string &name, const std::string &value, const std::string &category);
	void unsetValue(const std::string &group, const std::string &name);
	void setCategory(const std::string &group, const std::string &name, const std::string &category);

private:
  std::map<std::string,XMLConfigDeltaGroup*> myGroups;
  std::set<std::string> &myCategories;
};

#endif /* __XMLCONFIGDELTA_H__ */
