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

#ifndef __ZLCOLOROPTIONBUILDER_H__
#define __ZLCOLOROPTIONBUILDER_H__

#include <string>

#include <shared_ptr.h>
#include <ZLOptionEntry.h>

class ZLColorOptionBuilder {

public:
	ZLColorOptionBuilder();
	~ZLColorOptionBuilder();
	void addOption(const std::string &name, ZLColorOption &option);
	void setInitial(const std::string &name);
	ZLOptionEntry *comboEntry() const;
	ZLOptionEntry *colorEntry() const;

private:
	shared_ptr<class ZLColorOptionsData> myData;
};

#endif /* __ZLCOLOROPTIONBUILDER_H__ */
