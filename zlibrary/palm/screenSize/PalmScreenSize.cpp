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

#include <PalmOS.h>				

#include "PalmScreenSize.h"

PalmScreenSize::PalmScreenSize() {
}

PalmScreenSize::~PalmScreenSize() {
}

void PalmScreenSize::createInstance() {
	ourInstance = new PalmScreenSize();
}

static const UInt32 RomVersion50 = sysMakeROMVersion(5, 0, 0, sysROMStageDevelopment, 0);
static UInt32 romVersion = 0;

unsigned int PalmScreenSize::widthInternal() const {
	// TODO: implement
	if (romVersion == 0) {
		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	}
	if (romVersion < RomVersion50) {
		return 160;
	}
	return 320;
}

unsigned int PalmScreenSize::heightInternal() const {
	// TODO: implement
	if (romVersion == 0) {
		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	}
	if (romVersion < RomVersion50) {
		return 160;
	}
	return 320;
}
