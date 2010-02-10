/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLExecutionData.h>

#include "ZLImage.h"

#include "ZLImageManager.h"

shared_ptr<ZLExecutionData> ZLImage::synchronizationData() const {
	return 0;
}

bool ZLSingleImage::good() const {
	// FIXME: now imageData() always returns not null -- we need better image validation
	return !ZLImageManager::Instance().imageData(*this).isNull();
}

bool ZLMultiImage::good() const {
	const unsigned int w = rows();
	const unsigned int h = columns();
	for (unsigned int row = 0; row < w; ++row) {
		for (unsigned int col = 0; col < h; ++col) {
			shared_ptr<const ZLImage> img = subImage(row, col);
			if (img.isNull() || !img->good()) {
				return false;
			}
		}
	}
	return true;
}
