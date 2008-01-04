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

#ifndef __ZLSIMPLEOPTIONENTRY_H__
#define __ZLSIMPLEOPTIONENTRY_H__

#include <ZLOptionEntry.h>

class ZLSimpleStringOptionEntry : public ZLStringOptionEntry {

public:
	ZLSimpleStringOptionEntry(ZLStringOption &option);
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	ZLStringOption &myOption;
};

class ZLSimpleBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	ZLSimpleBooleanOptionEntry(ZLBooleanOption &option);
	bool initialState() const;
	void onAccept(bool state);

private:
	ZLBooleanOption &myOption;
};

class ZLSimpleSpinOptionEntry : public ZLSpinOptionEntry {

public:
	ZLSimpleSpinOptionEntry(ZLIntegerRangeOption &option, int step);
	int initialValue() const;
	int minValue() const;
	int maxValue() const;
	int step() const;
	void onAccept(int value);

private:
	ZLIntegerRangeOption &myOption;
	int myStep;
};

class ZLSimpleBoolean3OptionEntry : public ZLBoolean3OptionEntry {

public:
	ZLSimpleBoolean3OptionEntry(ZLBoolean3Option &option);
	ZLBoolean3 initialState() const;
	void onAccept(ZLBoolean3 state);

private:
	ZLBoolean3Option &myOption;
};

/*
class ZLSimpleColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLSimpleColorOptionEntry(ZLColorOption &option);
	const ZLColor color() const;
	void onAccept(ZLColor color);

private:
	ZLColorOption &myOption; 
};
*/

#endif /* __ZLSIMPLEOPTIONENTRY_H__ */
