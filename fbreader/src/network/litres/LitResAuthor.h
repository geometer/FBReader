/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESAUTHOR_H__
#define __LITRESAUTHOR_H__

#include <string>

struct LitResAuthor {
	std::string Id;
	std::string Title;
	std::string FirstName;
	std::string MiddleName;
	std::string LastName;

	LitResAuthor();
	LitResAuthor(const std::string &id, const std::string &title, const std::string &firstName, const std::string &middleName, const std::string &lastName);

	bool operator < (const LitResAuthor &other) const;
};

inline LitResAuthor::LitResAuthor() {
}
inline LitResAuthor::LitResAuthor(const std::string &id, const std::string &title, const std::string &firstName, const std::string &middleName, const std::string &lastName) : Id(id), Title(title), FirstName(firstName), MiddleName(middleName), LastName(lastName) {
}
inline bool LitResAuthor::operator < (const LitResAuthor &other) const {
	return LastName < other.LastName;
}

#endif /* __LITRESAUTHOR_H__ */
