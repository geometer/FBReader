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

#ifndef __XMLOPTIONS_H__
#define __XMLOPTIONS_H__

#include "../../abstract/options/ZLOptions_internal.h"

class XMLConfig;

class XMLOptions : public ZLOptions {

public:
	static void createInstance(const std::string &name, const std::string homeDirectory = "~") { ourInstance = new XMLOptions(name, homeDirectory); }

private:
	XMLOptions(const std::string &name, const std::string &homeDirectory);
	~XMLOptions();

public:
	void setGroup(const std::string &name);
	void clearGroup();

	void unsetValue(const std::string &name);

	bool booleanValue(const std::string &name, bool defaultValue);
	void setValue(const std::string &name, bool value, const std::string &category);

	long integerValue(const std::string &name, long defaultValue);
	void setValue(const std::string &name, long value, const std::string &category);

	double doubleValue(const std::string &name, double defaultValue);
	void setValue(const std::string &name, double value, const std::string &category);

	std::string stringValue(const std::string &name, const std::string &defaultValue);
	void setValue(const std::string &name, const std::string &value, const std::string &category);

	void setCategory(const std::string &name, const std::string &category);

private:
	XMLConfig *myConfig;
	std::string myGroupName;
};

#endif /* __XMLOPTIONS_H__ */
