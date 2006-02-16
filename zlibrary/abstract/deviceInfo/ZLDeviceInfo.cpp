/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLDeviceInfo.h"

ZLDeviceInfo *ZLDeviceInfo::ourInstance = 0;

ZLDeviceInfo::ZLDeviceInfo() {
}

ZLDeviceInfo::~ZLDeviceInfo() {
}

ZLDeviceInfo::ScreenSize ZLDeviceInfo::screenSize() {
	unsigned int s0 = ourInstance->screenWidthInternal();
	unsigned int s1 = ourInstance->screenHeightInternal();
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

bool ZLDeviceInfo::isMousePresented() {
	return screenSize() == SIZE_DESKTOP;
}

void ZLDeviceInfo::deleteInstance() {
	delete ourInstance;
}
