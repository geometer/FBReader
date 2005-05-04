/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PALMSCREENSIZE_H__
#define __PALMSCREENSIZE_H__

#include <abstract/ZLScreenSize.h>

class PalmScreenSize : public ZLScreenSize {

public:
	static void createInstance() UTIL_SECTION;

private:
	PalmScreenSize() UTIL_SECTION;

public:
	~PalmScreenSize() UTIL_SECTION;
	
protected:
	Size getSizeInternal() const UTIL_SECTION;
};

inline PalmScreenSize::PalmScreenSize() {}
inline PalmScreenSize::~PalmScreenSize() {}
inline void PalmScreenSize::createInstance() { ourInstance = new PalmScreenSize(); }

#endif /* __PALMSCREENSIZE_H__ */
