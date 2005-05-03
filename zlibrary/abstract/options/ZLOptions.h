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

#ifndef __ZLOPTIONS_H__
#define __ZLOPTIONS_H__

#include <string>

class ZLOption {

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

	bool value() const OPTIONS_SECTION;
	void setValue(bool value) const OPTIONS_SECTION;

private:
	mutable bool myValue;
	bool myDefaultValue;
};

enum Boolean3 {
	B3_FALSE = 0,
	B3_TRUE = 1,
	B3_UNDEFINED = 2
};

class ZLBoolean3Option : public ZLOption {

public:
	ZLBoolean3Option(const std::string &group, const std::string &optionName, Boolean3 defaultValue) OPTIONS_SECTION;

	Boolean3 value() const OPTIONS_SECTION;
	void setValue(Boolean3 value) const OPTIONS_SECTION;

private:
	mutable Boolean3 myValue;
	Boolean3 myDefaultValue;
};

class ZLIntegerOption : public ZLOption {

public:
	ZLIntegerOption(const std::string &group, const std::string &optionName, long defaultValue) OPTIONS_SECTION;

	long value() const OPTIONS_SECTION;
	void setValue(long value) const OPTIONS_SECTION;

private:
	mutable long myValue;
	long myDefaultValue;
};

struct ZLColor {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	ZLColor(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}
	ZLColor(long longValue = 0) : Red(longValue >> 16), Green((longValue >> 8) & 0xFF), Blue(longValue & 0xFF) {}
	long intValue() { return (((long)Red) << 16) + (((long)Green) << 8) + Blue; }
};

class ZLColorOption : public ZLOption {

public:
	ZLColorOption(const std::string &group, const std::string &optionName, ZLColor defaultValue) OPTIONS_SECTION;

	ZLColor value() const OPTIONS_SECTION;
	void setValue(ZLColor value) const OPTIONS_SECTION;

private:
	mutable long myIntValue;
	long myDefaultIntValue;
};

class ZLDoubleOption : public ZLOption {

public:
	ZLDoubleOption(const std::string &group, const std::string &optionName, double defaultValue) OPTIONS_SECTION;

	double value() const OPTIONS_SECTION;
	void setValue(double value) const OPTIONS_SECTION;

private:
	mutable double myValue;
	double myDefaultValue;
};

class ZLStringOption : public ZLOption {

public:
	ZLStringOption(const std::string &group, const std::string &optionName, const std::string &defaultValue) OPTIONS_SECTION;

	const std::string &value() const OPTIONS_SECTION;
	void setValue(const std::string &value) const OPTIONS_SECTION;

private:
	mutable std::string myValue;
	std::string myDefaultValue;
};

#endif /* __ZLOPTIONS_H__ */
