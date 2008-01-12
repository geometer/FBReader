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

#ifndef __ZLSIMPLEKEYOPTIONENTRY_H__
#define __ZLSIMPLEKEYOPTIONENTRY_H__

#include <map>
#include <vector>

#include <ZLOptionEntry.h>

class ZLKeyBindings;

class ZLSimpleKeyOptionEntry : public ZLKeyOptionEntry {

public:
	class CodeIndexBimap {

	public:
		void insert(const std::string &code);
		int indexByCode(const std::string &code) const;
		const std::string &codeByIndex(int index) const;

	private:
		std::vector<std::string> CodeByIndex;
		std::map<std::string,int> IndexByCode;
	};

public:
	ZLSimpleKeyOptionEntry(ZLKeyBindings &bindings);
	void onAccept();
	int actionIndex(const std::string &key);
	void onValueChanged(const std::string &key, int index);
	void onKeySelected(const std::string &key);
	virtual const CodeIndexBimap &codeIndexBimap() const = 0;

private:
	ZLKeyBindings &myBindings;
	std::map<std::string,std::string> myChangedCodes;
};

#endif /* __ZLSIMPLEKEYOPTIONENTRY_H__ */
