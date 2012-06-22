/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include "ZLSliceInputStream.h"

ZLSliceInputStream::ZLSliceInputStream(shared_ptr<ZLInputStream> base, size_t start, size_t length) :
	myBaseStream(base),
	myStart(start),
	myLength(length) {
}

bool ZLSliceInputStream::open() {
	if (!myBaseStream->open()) {
		return false;
	}
	myBaseStream->seek(myStart, true);
	return true;
}

size_t ZLSliceInputStream::read(char *buffer, size_t maxSize) {
	const int maxbytes = myLength - offset();
	if (maxbytes <= 0) {
		return 0;
	}
	size_t res = myBaseStream->read(buffer, std::min(maxSize, (size_t)maxbytes));
	return res;
}

void ZLSliceInputStream::close() {
	return myBaseStream->close();
}

void ZLSliceInputStream::seek(int off, bool absoluteOffset) {
	if (absoluteOffset) {
		myBaseStream->seek(myStart + std::min(off, (int)(myLength)), true);
	} else {
		myBaseStream->seek(std::min(off, (int)(myLength - offset())), false);
	}
}

size_t ZLSliceInputStream::offset() const {
	return myBaseStream->offset() - myStart;
}

size_t ZLSliceInputStream::sizeOfOpened() {
	return myBaseStream->sizeOfOpened();
}
