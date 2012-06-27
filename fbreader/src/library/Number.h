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

#ifndef __NUMBER_H__
#define __NUMBER_H__

#include <string>

class Number {
public:
	Number();
	Number(std::string number);
	Number(int number);

	static std::string validate(const std::string& value);

	const std::string &value() const;
	void setValue(const std::string &value);

	//TODO implement validation

	bool operator< (const Number &number) const;
	bool operator== (const Number &number) const;

private:
	std::string myNumber;
};

#endif /* __NUMBER_H__ */
