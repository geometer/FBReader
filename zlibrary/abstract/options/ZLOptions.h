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
	static const std::string LOOK_AND_FEEL_CATEGORY;
	static const std::string CONFIG_CATEGORY;
	static const std::string STATE_CATEGORY;

public:
	static void clearGroup(const std::string &group);
	static void startAutoSave(int seconds);
	
public:
	ZLOption(const std::string &category, const std::string &group, const std::string &optionName);
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

class ZLBooleanOption : public ZLOption {

public:
	ZLBooleanOption(const std::string &category, const std::string &group, const std::string &optionName, bool defaultValue);
	~ZLBooleanOption();

	bool value() const;
	void setValue(bool value);

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
	ZLBoolean3Option(const std::string &category, const std::string &group, const std::string &optionName, Boolean3 defaultValue);
	~ZLBoolean3Option();

	Boolean3 value() const;
	void setValue(Boolean3 value);

private:
	mutable Boolean3 myValue;
	const Boolean3 myDefaultValue;
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

struct ZLColor {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	ZLColor(unsigned char r, unsigned char g, unsigned char b);
	ZLColor(long longValue = 0);
	~ZLColor();
	long intValue();
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

class ZLStringOption : public ZLOption {

public:
	ZLStringOption(const std::string &category, const std::string &group, const std::string &optionName, const std::string &defaultValue);
	~ZLStringOption();

	const std::string &value() const;
	void setValue(const std::string &value);

private:
	mutable std::string myValue;
	const std::string myDefaultValue;
};

inline ZLColor::ZLColor(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}
inline ZLColor::ZLColor(long longValue) : Red(longValue >> 16), Green((longValue >> 8) & 0xFF), Blue(longValue & 0xFF) {}
inline ZLColor::~ZLColor() {}
inline long ZLColor::intValue() { return (((long)Red) << 16) + (((long)Green) << 8) + Blue; }

#endif /* __ZLOPTIONS_H__ */
