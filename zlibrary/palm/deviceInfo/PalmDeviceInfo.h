/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __PALMDEVICEINFO_H__
#define __PALMDEVICEINFO_H__

#include <abstract/ZLDeviceInfo.h>

class PalmDeviceInfo : public ZLDeviceInfo {

public:
	static void createInstance() UTIL_SECTION;

private:
	PalmDeviceInfo() UTIL_SECTION;

public:
	~PalmDeviceInfo() UTIL_SECTION;
	
protected:
	unsigned int screenWidthInternal() const UTIL_SECTION;
	unsigned int screenHeightInternal() const UTIL_SECTION;
	bool isKeyboardPresentedInternal() const UTIL_SECTION;
};

#endif /* __PALMDEVICEINFO_H__ */
