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

#ifndef __ZLSIMPLEOPTIONENTRY_H__
#define __ZLSIMPLEOPTIONENTRY_H__

#include <ZLOptionEntry.h>

class ZLSimpleStringOptionEntry : public ZLStringOptionEntry {

public:
	ZLSimpleStringOptionEntry(const std::string &name, ZLStringOption &option);
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	const std::string myName;
	ZLStringOption &myOption;
};

class ZLSimpleBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	ZLSimpleBooleanOptionEntry(const std::string &name, ZLBooleanOption &option);
	const std::string &name() const;
	bool initialState() const;
	void onAccept(bool state);

private:
	const std::string myName;
	ZLBooleanOption &myOption;
};

class ZLSimpleSpinOptionEntry : public ZLSpinOptionEntry {

public:
	ZLSimpleSpinOptionEntry(const std::string &name, ZLIntegerRangeOption &option, int step);
	const std::string &name() const;
	int initialValue() const;
	int minValue() const;
	int maxValue() const;
	int step() const;
	void onAccept(int value);

private:
	const std::string myName;
	ZLIntegerRangeOption &myOption;
	int myStep;
};

class ZLSimpleBoolean3OptionEntry : public ZLComboOptionEntry {

public:
	ZLSimpleBoolean3OptionEntry(const std::string &name, ZLBoolean3Option &option);
	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	static std::vector<std::string> ourValues;
	const std::string myName;
	ZLBoolean3Option &myOption;
};

class ZLSimpleColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLSimpleColorOptionEntry(const std::string &name, ZLColorOption &option);
	const std::string &name() const;
	const ZLColor color() const;
	void onAccept(ZLColor color);

private:
	const std::string myName;
	ZLColorOption &myOption; 
};

#endif /* __ZLSIMPLEOPTIONENTRY_H__ */
