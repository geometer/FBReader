/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLOPTIONS_INTERNAL_H__
#define __ZLOPTIONS_INTERNAL_H__

#include <string>

class ZLOptions {

protected:
	static ZLOptions *ourInstance;

protected:
	ZLOptions() {}
	virtual ~ZLOptions() {}

public:
	static ZLOptions &instance() { return *ourInstance; }
	static void deleteInstance() { delete ourInstance; }

	virtual void setGroup(const std::string &name) = 0;
	virtual void clearGroup() = 0;

	virtual void unsetValue(const std::string &name) = 0;

	virtual bool booleanValue(const std::string &name, bool defaultValue) = 0;
	virtual void setValue(const std::string &name, bool value) = 0;

	virtual int integerValue(const std::string &name, int defaultValue) = 0;
	virtual void setValue(const std::string &name, int value) = 0;

	virtual double doubleValue(const std::string &name, double defaultValue) = 0;
	virtual void setValue(const std::string &name, double value) = 0;

	virtual std::string stringValue(const std::string &name, const std::string &defaultValue) = 0;
	virtual void setValue(const std::string &name, const std::string &value) = 0;
};

#endif /* __ZLOPTIONS_INTERNAL_H__ */
