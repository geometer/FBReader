/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <cstdlib>

#include <ZLStringUtil.h>

#include "Number.h"

static std::string ZERO = "0";

Number::Number() : myNumber(ZERO) {
}

Number::Number(std::string number) : myNumber(validate(number)) {
}

Number::Number(int number) {
	ZLStringUtil::appendNumber(myNumber, number);
}

std::string Number::validate(const std::string &value) {
	std::string result = value;
	while (result.size() > 1 && ZLStringUtil::stringStartsWith(result, ZERO)) {
		result = result.substr(1);
	}
	return result;
}

const std::string &Number::value() const {
	return myNumber;
}

void Number::setValue(const std::string &value) {
	myNumber = validate(value);
}

bool Number::operator <(const Number &number) const {
	int index = strtol(myNumber.c_str(), 0, 10);
	int otherIndex = strtol(number.value().c_str(), 0, 10);
	return index < otherIndex;
}

bool Number::operator ==(const Number &number) const {
	int index = strtol(myNumber.c_str(), 0, 10);
	int otherIndex = strtol(number.value().c_str(), 0, 10);
	return index == otherIndex;
}
