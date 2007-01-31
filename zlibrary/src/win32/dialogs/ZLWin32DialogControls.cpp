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

#include "ZLWin32DialogControl.h"

const std::string ZLWin32PushButton::ClassName = "button";

ZLWin32PushButton::ZLWin32PushButton(int x, int y, int width, int height, WORD id, const std::string &text) : ZLWin32DialogControl(BS_PUSHBUTTON, x, y, width, height, id, ClassName, text) {
	//DWORD style = (it == myButtons.begin()) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
}
