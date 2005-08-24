/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __XMLCONFIG_H__
#define __XMLCONFIG_H__

#include <map>
#include <string>

class XMLConfigGroup {

public:
	const std::map<std::string,std::string> &values() const { return myValues; }
	const std::string &getValue(const std::string &name, const std::string &defaultValue) const;
	void setValue(const std::string &name, const std::string &value) { myValues[name] = value; }
	void unsetValue(const std::string &name);

private:
	std::map<std::string,std::string> myValues;
};

class XMLConfig {

public:
	XMLConfig(const std::string &name);
	~XMLConfig();

	XMLConfigGroup *getGroup(const std::string &name, bool createUnexisting);
	void removeGroup(const std::string &name);

private:
	std::string myName;
	std::map<std::string,XMLConfigGroup*> myGroups;

friend class XMLConfigReader;
friend class XMLConfigWriter;
};

#endif /* __XMLCONFIG_H__ */
