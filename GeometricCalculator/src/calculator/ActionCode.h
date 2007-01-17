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

#ifndef __ACTIONCODE_H__
#define __ACTIONCODE_H__

enum ActionCode {
	NO_ACTION = 0,
	MODE_ADD_POINT = 1,
	MODE_ADD_POINT_ON_THE_LINE = 2,
	MODE_ADD_MIDDLE_POINT = 3,
	MODE_ADD_LINE = 4,
	MODE_ADD_HALFLINE = 5,
	MODE_ADD_SEGMENT = 6,
	MODE_ADD_PERPENDICULAR = 7,
	MODE_COPY_OBJECT = 8,
	MODE_ADD_CIRCLE = 9,
	MODE_ADD_RULER = 10,
	MODE_MOVE_POINT = 11,
	MODE_DELETE = 12,
	MODE_NO_ACTION = 13,
	ACTION_UNDO = 14,
	ACTION_REDO = 15,
	ACTION_SHOW_INFO = 16,
	ACTION_SHOW_OPTIONS = 17,
	ACTION_NEW_SCENE = 18,
	ACTION_OPEN_SCENE = 19,
	ACTION_QUIT = 20,
	ACTION_ZOOM_IN = 21,
	ACTION_ZOOM_OUT = 22,
	ACTION_SAVE_SCENE = 23,
	ACTION_REMOVE_SCENES = 24,
	MODE_EDIT_OBJECT = 25,
	ACTION_SET_DEFAULT_ZOOM = 26,
};

#endif /* __ACTIONCODE_H__ */
