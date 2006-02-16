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

#ifndef __PALMOPTIONS_H__
#define __PALMOPTIONS_H__

#include "../../abstract/options/ZLOptions_internal.h"

// TODO: write real implementation
class PalmOptions : public ZLOptions {

public:
	static void createInstance(const char *name) OPTIONS_SECTION;
	
private:
	PalmOptions(const char* name) OPTIONS_SECTION;

public:
	~PalmOptions() OPTIONS_SECTION;

public:
	void setGroup(const std::string &name) OPTIONS_SECTION;
	void clearGroup() OPTIONS_SECTION;
	
	void unsetValue(const std::string &name) OPTIONS_SECTION;

	bool booleanValue(const std::string &name, bool defaultValue) OPTIONS_SECTION;
	void setValue(const std::string &name, bool value) OPTIONS_SECTION;

	long integerValue(const std::string &name, long defaultValue) OPTIONS_SECTION;
	void setValue(const std::string &name, long value) OPTIONS_SECTION;

	double doubleValue(const std::string &name, double defaultValue) OPTIONS_SECTION;
	void setValue(const std::string &name, double value) OPTIONS_SECTION;

	std::string stringValue(const std::string &name, const std::string &defaultValue) OPTIONS_SECTION;
	void setValue(const std::string &name, const std::string &value) OPTIONS_SECTION;
};

#endif  __PALMOPTIONS_H__ 
