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
	ZLOption(const std::string &group, const std::string &optionName);
	virtual ~ZLOption();

protected:
	std::string myGroup;
	std::string myOptionName;
	mutable bool myIsSynchronized;

private:
	ZLOption(const ZLOption&);
	const ZLOption& operator = (const ZLOption&);
};

class ZLBooleanOption : public ZLOption {

public:
	ZLBooleanOption(const std::string &group, const std::string &optionName, bool defaultValue);

	bool value() const;
	void setValue(bool value) const;

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
	ZLBoolean3Option(const std::string &group, const std::string &optionName, Boolean3 defaultValue);

	Boolean3 value() const;
	void setValue(Boolean3 value) const;

private:
	mutable Boolean3 myValue;
	Boolean3 myDefaultValue;
};

class ZLIntegerOption : public ZLOption {

public:
	ZLIntegerOption(const std::string &group, const std::string &optionName, int defaultValue);

	int value() const;
	void setValue(int value) const;

private:
	mutable int myValue;
	int myDefaultValue;
};

struct ZLColor {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	ZLColor(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}
	ZLColor(int intValue = 0) : Red(intValue >> 16), Green((intValue >> 8) & 0xFF), Blue(intValue & 0xFF) {}
	int intValue() { return (((int)Red) << 16) + (((int)Green) << 8) + Blue; }
};

class ZLColorOption : public ZLOption {

public:
	ZLColorOption(const std::string &group, const std::string &optionName, ZLColor defaultValue);

	ZLColor value() const;
	void setValue(ZLColor value) const;

private:
	mutable int myIntValue;
	int myDefaultIntValue;
};

class ZLDoubleOption : public ZLOption {

public:
	ZLDoubleOption(const std::string &group, const std::string &optionName, double defaultValue);

	double value() const;
	void setValue(double value) const;

private:
	mutable double myValue;
	double myDefaultValue;
};

class ZLStringOption : public ZLOption {

public:
	ZLStringOption(const std::string &group, const std::string &optionName, const std::string &defaultValue);

	const std::string value() const;
	void setValue(const std::string &value) const;

private:
	mutable std::string myValue;
	std::string myDefaultValue;
};

#endif /* __ZLOPTIONS_H__ */
