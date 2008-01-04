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

#include "ZLWin32ImageManager.h"

bool ZLWin32ImageManager::bmpConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	if (stringData.size() <= sizeof(BITMAPFILEHEADER)) {
		return false;
	}
	const BITMAPFILEHEADER &fileHeader = *(const BITMAPFILEHEADER*)stringData.data();
	if ((fileHeader.bfType != 0x4d42) || (fileHeader.bfSize != stringData.size())) {
		return false;
	}

	data.clear();
	const int infoSize = fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	data.myInfo = (BITMAPINFO*)new char[infoSize];
	memcpy(data.myInfo, stringData.data() + sizeof(BITMAPFILEHEADER), infoSize);
	const int arraySize = fileHeader.bfSize - fileHeader.bfOffBits;
	data.myArray = new BYTE[arraySize];
	memcpy(data.myArray, stringData.data() + fileHeader.bfOffBits, arraySize);

	data.myWidth = data.myInfo->bmiHeader.biWidth;
	data.myHeight = data.myInfo->bmiHeader.biHeight;
	data.myBytesPerPixel = 3;
	data.myBytesPerLine = (data.myWidth * 24 + 31) / 32 * 4;

	return true;
}
