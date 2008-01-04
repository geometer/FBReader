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

#ifndef __ZLRESOURCE_H__
#define __ZLRESOURCE_H__

#include <string>

struct ZLResourceKey {
	ZLResourceKey() {}
	explicit ZLResourceKey(const std::string &name) : Name(name) {}

	std::string Name;
};

class ZLResource {

public:
	static const ZLResource &resource(const std::string &key);
	static const ZLResource &resource(const ZLResourceKey &key);

protected:
	ZLResource(const std::string &name);

public:
	virtual ~ZLResource();
	const std::string &name() const;
	virtual bool hasValue() const = 0;
	virtual const std::string &value() const = 0;
	virtual const ZLResource &operator [] (const std::string &key) const = 0;
	const ZLResource &operator [] (const ZLResourceKey &key) const;

private:
	const std::string myName;

private:
	// disable copying
	ZLResource(const ZLResource&);
	const ZLResource &operator = (const ZLResource&);
};

#endif /* __ZLRESOURCE_H__ */
