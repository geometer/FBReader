/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>

#include "CHMFileImage.h"

CHMFileImage::CHMFileImage(shared_ptr<CHMFileInfo> info, const std::string &entry) : ZLStreamImage("image/auto", 0, 0), myInfo(info), myEntry(entry) {
}

shared_ptr<ZLInputStream> CHMFileImage::inputStream() const {
	shared_ptr<ZLInputStream> baseStream = ZLFile(myInfo->fileName()).inputStream();
	if (baseStream.isNull() || !baseStream->open()) {
		return 0;
	}
	return myInfo->entryStream(baseStream, myEntry);
}
