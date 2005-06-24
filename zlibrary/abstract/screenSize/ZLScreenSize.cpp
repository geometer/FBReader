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

#include "ZLScreenSize.h"

ZLScreenSize *ZLScreenSize::ourInstance = 0;

ZLScreenSize::ZLScreenSize() {
}

ZLScreenSize::~ZLScreenSize() {
}

ZLScreenSize::Size ZLScreenSize::size() {
	unsigned int s0 = ourInstance->widthInternal();
	unsigned int s1 = ourInstance->heightInternal();
	switch (s0 * s1) {
		case 160 * 160:
			return SIZE_160x160;
		case 240 * 320:
			return SIZE_240x320;
		case 320 * 320:
			return SIZE_320x320;
		case 640 * 480:
			return SIZE_640x480;
		case 800 * 480:
			return SIZE_800x480;
		default:
			return SIZE_DESKTOP;
	}
}

void ZLScreenSize::deleteInstance() {
	delete ourInstance;
}
