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

#ifndef __ZLOPTIONS_H__
#define __ZLOPTIONS_H__

#include <string>

class ZLOption {

public:
	static void clearGroup(const std::string &group) OPTIONS_SECTION;
	
public:
	ZLOption(const std::string &group, const std::string &optionName) OPTIONS_SECTION;
	virtual ~ZLOption() OPTIONS_SECTION;

protected:
	std::string myGroup;
	std::string myOptionName;
	mutable bool myIsSynchronized;

private:
	ZLOption(const ZLOption&) OPTIONS_SECTION;
	const ZLOption& operator = (const ZLOption&) OPTIONS_SECTION;
};

class ZLBooleanOption : public ZLOption {

public:
	ZLBooleanOption(const std::string &group, const std::string &optionName, bool defaultValue) OPTIONS_SECTION;
	~ZLBooleanOption() OPTIONS_SECTION;

	bool value() const OPTIONS_SECTION;
	void setValue(bool value) OPTIONS_SECTION;

private:
	mutable bool myValue;
	const bool myDefaultValue;
};

enum Boolean3 {
	B3_FALSE = 0,
	B3_TRUE = 1,
	B3_UNDEFINED = 2
};

class ZLBoolean3Option : public ZLOption {

public:
	ZLBoolean3Option(const std::string &group, const std::string &optionName, Boolean3 defaultValue) OPTIONS_SECTION;
	~ZLBoolean3Option() OPTIONS_SECTION;

	Boolean3 value() const OPTIONS_SECTION;
	void setValue(Boolean3 value) OPTIONS_SECTION;

private:
	mutable Boolean3 myValue;
	const Boolean3 myDefaultValue;
};

class ZLIntegerOption : public ZLOption {

public:
	ZLIntegerOption(const std::string &group, const std::string &optionName, long defaultValue) OPTIONS_SECTION;
	~ZLIntegerOption() OPTIONS_SECTION;

	long value() const OPTIONS_SECTION;
	void setValue(long value) OPTIONS_SECTION;

private:
	mutable long myValue;
	const long myDefaultValue;
};

class ZLIntegerRangeOption : public ZLOption {

public:
	ZLIntegerRangeOption(const std::string &group, const std::string &optionName, long minValue, long maxValue, long defaultValue) OPTIONS_SECTION;
	~ZLIntegerRangeOption() OPTIONS_SECTION;

	long value() const OPTIONS_SECTION;
	void setValue(long value) OPTIONS_SECTION;
	long minValue() const OPTIONS_SECTION;
	long maxValue() const OPTIONS_SECTION;

private:
	mutable long myValue;
	const long myMinValue;
	const long myMaxValue;
	const long myDefaultValue;
};

struct ZLColor {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	ZLColor(unsigned char r, unsigned char g, unsigned char b) OPTIONS_SECTION;
	ZLColor(long longValue = 0) OPTIONS_SECTION;
	~ZLColor() OPTIONS_SECTION;
	long intValue() OPTIONS_SECTION;
};

class ZLColorOption : public ZLOption {

public:
	ZLColorOption(const std::string &group, const std::string &optionName, ZLColor defaultValue) OPTIONS_SECTION;
	~ZLColorOption() OPTIONS_SECTION;

	ZLColor value() const OPTIONS_SECTION;
	void setValue(ZLColor value) OPTIONS_SECTION;

private:
	mutable long myIntValue;
	const long myDefaultIntValue;
};

class ZLDoubleOption : public ZLOption {

public:
	ZLDoubleOption(const std::string &group, const std::string &optionName, double defaultValue) OPTIONS_SECTION;
	~ZLDoubleOption() OPTIONS_SECTION;

	double value() const OPTIONS_SECTION;
	void setValue(double value) OPTIONS_SECTION;

private:
	mutable double myValue;
	const double myDefaultValue;
};

class ZLStringOption : public ZLOption {

public:
	ZLStringOption(const std::string &group, const std::string &optionName, const std::string &defaultValue) OPTIONS_SECTION;
	~ZLStringOption() OPTIONS_SECTION;

	const std::string &value() const OPTIONS_SECTION;
	void setValue(const std::string &value) OPTIONS_SECTION;

private:
	mutable std::string myValue;
	const std::string myDefaultValue;
};

inline ZLColor::ZLColor(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}
inline ZLColor::ZLColor(long longValue) : Red(longValue >> 16), Green((longValue >> 8) & 0xFF), Blue(longValue & 0xFF) {}
inline ZLColor::~ZLColor() {}
inline long ZLColor::intValue() { return (((long)Red) << 16) + (((long)Green) << 8) + Blue; }

#endif /* __ZLOPTIONS_H__ */
