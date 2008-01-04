/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __W32CONTROLCOLLECTION_H__
#define __W32CONTROLCOLLECTION_H__

#include <map>

class W32Control;

class W32ControlCollection {

protected:
	W32ControlCollection(int startId);
	virtual ~W32ControlCollection();
	W32Control *operator[] (short id);

public:
	short addControl(W32Control *control, short id = -1);	
	virtual void invalidate() = 0;

private:
	short myCurrentId;
	std::map<short,W32Control*> myControlByIdMap;
};

#endif /* __W32CONTROLCOLLECTION_H__ */
