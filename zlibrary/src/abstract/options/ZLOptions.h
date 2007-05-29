/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLOPTIONS_H__
#define __ZLOPTIONS_H__

#include <string>

#include <ZLColor.h>
#include <ZLBoolean3.h>

class ZLOption {

public:
	static const std::string LOOK_AND_FEEL_CATEGORY;
	static const std::string CONFIG_CATEGORY;
	static const std::string STATE_CATEGORY;

public:
	static void clearGroup(const std::string &group);
	static bool isAutoSavingSupported();
	static void startAutoSave(int seconds);
	
protected:
	ZLOption(const std::string &category, const std::string &group, const std::string &optionName);

public:
	virtual ~ZLOption();

protected:
	std::string myCategory;
	std::string myGroup;
	std::string myOptionName;
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
	ZLSimpleOption(const std::string &category, const std::string &group, const std::string &optionName);

public:
	virtual Type type() const = 0;
};

class ZLBooleanOption : public ZLSimpleOption {

public:
	ZLBooleanOption(const std::string &category, const std::string &group, const std::string &optionName, bool defaultValue);
	~ZLBooleanOption();
	Type type() const;

	bool value() const;
	void setValue(bool value);

private:
	mutable bool myValue;
	const bool myDefaultValue;
};

class ZLBoolean3Option : public ZLSimpleOption {

public:
	ZLBoolean3Option(const std::string &category, const std::string &group, const std::string &optionName, ZLBoolean3 defaultValue);
	~ZLBoolean3Option();
	Type type() const;

	ZLBoolean3 value() const;
	void setValue(ZLBoolean3 value);

private:
	mutable ZLBoolean3 myValue;
	const ZLBoolean3 myDefaultValue;
};

class ZLIntegerOption : public ZLOption {

public:
	ZLIntegerOption(const std::string &category, const std::string &group, const std::string &optionName, long defaultValue);
	~ZLIntegerOption();

	long value() const;
	void setValue(long value);

private:
	mutable long myValue;
	const long myDefaultValue;
};

class ZLIntegerRangeOption : public ZLOption {

public:
	ZLIntegerRangeOption(const std::string &category, const std::string &group, const std::string &optionName, long minValue, long maxValue, long defaultValue);
	~ZLIntegerRangeOption();

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
	ZLColorOption(const std::string &category, const std::string &group, const std::string &optionName, ZLColor defaultValue);
	~ZLColorOption();

	ZLColor value() const;
	void setValue(ZLColor value);

private:
	mutable long myIntValue;
	const long myDefaultIntValue;
};

class ZLDoubleOption : public ZLOption {

public:
	ZLDoubleOption(const std::string &category, const std::string &group, const std::string &optionName, double defaultValue);
	~ZLDoubleOption();

	double value() const;
	void setValue(double value);

private:
	mutable double myValue;
	const double myDefaultValue;
};

class ZLStringOption : public ZLSimpleOption {

public:
	ZLStringOption(const std::string &category, const std::string &group, const std::string &optionName, const std::string &defaultValue);
	~ZLStringOption();
	Type type() const;

	const std::string &value() const;
	void setValue(const std::string &value);

private:
	mutable std::string myValue;
	const std::string myDefaultValue;
};

#endif /* __ZLOPTIONS_H__ */
