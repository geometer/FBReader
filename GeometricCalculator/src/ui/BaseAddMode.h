/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __BASEADDMODE_H__
#define __BASEADDMODE_H__

#include "EditMode.h"
#include "../model/Object.h"

class BaseAddMode : public EditMode {

protected:
	BaseAddMode(DiagramView &view);
	PointPtr getPoint(ObjectPtr object0, ObjectPtr object1, int x, int y, bool createNewPoint);
	PointPtr getPoint(int x, int y, bool createNewPoint);

protected:
	ObjectPtr mySelectedObject0;
	ObjectPtr mySelectedObject1;
};

#endif /* __BASEADDMODE_H__ */
