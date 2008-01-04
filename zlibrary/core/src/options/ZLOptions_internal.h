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

#ifndef __ZLOPTIONS_INTERNAL_H__
#define __ZLOPTIONS_INTERNAL_H__

#include <string>

class ZLConfig;

class ZLOptions {

private:
	static ZLOptions *ourInstance;

private:
	ZLOptions();
	virtual ~ZLOptions();

public:
	static ZLOptions &instance();
	static void createInstance() { ourInstance = new ZLOptions(); }
	static void deleteInstance();

	virtual void setGroup(const std::string &name);
	virtual void clearGroup();

	virtual void unsetValue(const std::string &name);

	virtual bool booleanValue(const std::string &name, bool defaultValue);
	virtual void setValue(const std::string &name, bool value, const std::string &category);

	virtual long integerValue(const std::string &name, long defaultValue);
	virtual void setValue(const std::string &name, long value, const std::string &category);

	virtual double doubleValue(const std::string &name, double defaultValue);
	virtual void setValue(const std::string &name, double value, const std::string &category);

	virtual std::string stringValue(const std::string &name, const std::string &defaultValue);
	virtual void setValue(const std::string &name, const std::string &value, const std::string &category);

	virtual void startAutoSave(int seconds);
	virtual bool isAutoSavingSupported() const;

private:
	ZLConfig *myConfig;
	std::string myGroupName;
};

#endif /* __ZLOPTIONS_INTERNAL_H__ */
