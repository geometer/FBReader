/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __REGISTRYUTIL_H__
#define __REGISTRYUTIL_H__

#include <set>
#include <string>

#include <windows.h>

class RegistryUtil {

public:
	RegistryUtil();
	~RegistryUtil();

	std::string rootKeyName() const;

	void collectSubKeys(std::set<std::string> &keySet, HKEY root);
	void collectValues(std::set<std::string> &valueSet, HKEY key);
	bool getValue(std::string &value, HKEY key, const std::string &name);

	void removeValue(const std::string &keyName, const std::string &valueName);
	void setValue(const std::string &keyName, const std::string &valueName, const std::string &value);

private:
	const int myBufferSize;
	char *myBuffer;
};

#endif /* __REGISTRYUTIL_H__ */
