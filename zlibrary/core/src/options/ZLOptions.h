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

#ifndef __ZLOPTIONS_H__
#define __ZLOPTIONS_H__

#include <vector>
#include <string>

#include <ZLColor.h>
#include <ZLBoolean3.h>

class ZLCategoryKey {

public:
	static const ZLCategoryKey EMPTY;
	static const ZLCategoryKey LOOK_AND_FEEL;
	static const ZLCategoryKey CONFIG;
	static const ZLCategoryKey STATE;
	static const ZLCategoryKey SYSTEM;

protected:
	explicit ZLCategoryKey(const std::string &name);

public:
	bool operator == (const ZLCategoryKey &key) const;
	bool operator != (const ZLCategoryKey &key) const;

	const std::string Name;
};

class ZLConfig;

class ZLOption {

private:
	static ZLConfig *ourConfig;

public:
	static void createInstance();
	static void deleteInstance();

protected:
	const std::string &getConfigValue(const std::string &defaultValue) const;
	const std::string &getConfigValue() const;
	const std::string &getDefaultConfigValue(const std::string &defaultValue) const;
	const std::string &getDefaultConfigValue() const;
	void setConfigValue(const std::string &value) const;
	void unsetConfigValue() const;

public:
	static const std::string PLATFORM_GROUP;
	static const std::string FULL_KEYBOARD_CONTROL;
	static const std::string KEYBOARD_PRESENTED;
	static const std::string MOUSE_PRESENTED;
	static const std::string TOUCHSCREEN_PRESENTED;
	static const std::string FINGER_TAP_DETECTABLE;

public:
	static void clearGroup(const std::string &group);
	static void listOptionNames(const std::string &group, std::vector<std::string> &names);
	static void listOptionGroups(std::vector<std::string> &groups);
	static bool isAutoSavingSupported();
	static void startAutoSave(int seconds);
	
protected:
	ZLOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName);

public:
	virtual ~ZLOption();

private:
	const ZLCategoryKey &myCategory;
	std::string myGroupName;
	std::string myOptionName;

protected:
	mutable bool myIsSynchronized;

private:
	ZLOption(const ZLOption&);
	const ZLOption& operator = (const ZLOption&);
};

class ZLSimpleOption : public ZLOption {

public:
	enum Type {
		TYPE_BOOLEAN,
		TYPE_BOOLEAN3,
		//TYPE_COLOR,
		TYPE_STRING,
	};

protected:
	ZLSimpleOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName);

public:
	virtual Type type() const = 0;
};

class ZLBooleanOption : public ZLSimpleOption {

public:
	ZLBooleanOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, bool defaultValue);
	Type type() const;

	bool value() const;
	void setValue(bool value);

private:
	mutable bool myValue;
	const bool myDefaultValue;
};

class ZLBoolean3Option : public ZLSimpleOption {

public:
	ZLBoolean3Option(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, ZLBoolean3 defaultValue);
	Type type() const;

	ZLBoolean3 value() const;
	void setValue(ZLBoolean3 value);

private:
	mutable ZLBoolean3 myValue;
	const ZLBoolean3 myDefaultValue;
};

class ZLIntegerOption : public ZLOption {

public:
	ZLIntegerOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, long defaultValue);

	long value() const;
	void setValue(long value);

private:
	mutable long myValue;
	const long myDefaultValue;
};

class ZLIntegerRangeOption : public ZLOption {

public:
	ZLIntegerRangeOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, long minValue, long maxValue, long defaultValue);

	long value() const;
	void setValue(long value);
	long minValue() const;
	long maxValue() const;

private:
	mutable long myValue;
	const long myMinValue;
	const long myMaxValue;
	const long myDefaultValue;
};

class ZLColorOption : public ZLOption {

public:
	ZLColorOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, ZLColor defaultValue);

	ZLColor value() const;
	void setValue(ZLColor value);

private:
	mutable long myIntValue;
	const long myDefaultIntValue;
};

class ZLDoubleOption : public ZLOption {

public:
	ZLDoubleOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, double defaultValue);

	double value() const;
	void setValue(double value);

private:
	mutable double myValue;
	const double myDefaultValue;
};

class ZLStringOption : public ZLSimpleOption {

public:
	ZLStringOption(const ZLCategoryKey &category, const std::string &group, const std::string &optionName, const std::string &defaultValue);
	Type type() const;

	const std::string &value() const;
	void setValue(const std::string &value);

private:
	mutable std::string myValue;
	const std::string myDefaultValue;
};

#endif /* __ZLOPTIONS_H__ */
