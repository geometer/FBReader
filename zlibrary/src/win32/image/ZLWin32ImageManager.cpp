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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ZLWin32ImageManager.h"

ZLWin32ImageData::ZLWin32ImageData() : myWidth(0), myHeight(0), myArray(0), myArrayWithoutAlpha(0), myFailure(false) {
}

ZLWin32ImageData::~ZLWin32ImageData() {
	if (myArray != 0) {
		delete[] myArray;
	}
	if (myArrayWithoutAlpha != 0) {
		delete[] myArrayWithoutAlpha;
	}
}

unsigned int ZLWin32ImageData::width() const {
	return myWidth;
}

unsigned int ZLWin32ImageData::height() const {
	return myHeight;
}

void ZLWin32ImageData::init(unsigned int width, unsigned int height) {
	init(width, height, false, 0);
}

void ZLWin32ImageData::init(unsigned int width, unsigned int height, bool hasAlpha, unsigned int bytesPerLine) {
	myWidth = width;
	myHeight = height;
	myBytesPerPixel = hasAlpha ? 4 : 3;
	myBytesPerLine = std::max(bytesPerLine, (myBytesPerPixel * myWidth + 3) >> 2 << 2);
	if (myArray != 0) {
		delete[] myArray;
	}
	if (myArrayWithoutAlpha != 0) {
		delete[] myArrayWithoutAlpha;
		myArrayWithoutAlpha = 0;
	}
	myArray = new BYTE[myBytesPerLine * myHeight];
	myPixelPointer = myArray;

	myInfo.bmiHeader.biSize = sizeof(myInfo.bmiHeader);
	myInfo.bmiHeader.biWidth = width;
	myInfo.bmiHeader.biHeight = height;
	myInfo.bmiHeader.biPlanes = 1;
	myInfo.bmiHeader.biBitCount = 24;
	myInfo.bmiHeader.biCompression = BI_RGB;
	myInfo.bmiHeader.biSizeImage = 0;
	myInfo.bmiHeader.biXPelsPerMeter = 100; //?
	myInfo.bmiHeader.biYPelsPerMeter = 100; //? 
	myInfo.bmiHeader.biClrUsed = 0;
	myInfo.bmiHeader.biClrImportant = 0;
	myInfo.bmiColors[0].rgbBlue = 1;
	myInfo.bmiColors[0].rgbGreen = 1;
	myInfo.bmiColors[0].rgbRed = 1;
	myInfo.bmiColors[0].rgbReserved = 0;
}

void ZLWin32ImageData::bgr2rgb() {
	if (myArray != 0) {
		for (unsigned int i = 0; i < myHeight; ++i) {
			BYTE *ptr = myArray + myBytesPerLine * i;
			for (unsigned int j = 0; j < myWidth; ++j, ptr += myBytesPerPixel) {
				BYTE b = ptr[0];
				ptr[0] = ptr[2];
				ptr[2] = b;
			}
		}
	}
}

void ZLWin32ImageData::setPosition(unsigned int x, unsigned int y) {
	myPixelPointer = myArray + myBytesPerLine * (myHeight - y - 1) + myBytesPerPixel * x;
}

void ZLWin32ImageData::moveX(int delta) {
	myPixelPointer += myBytesPerPixel * delta;
}

void ZLWin32ImageData::moveY(int delta) {
	myPixelPointer -= myBytesPerLine * delta;
}

void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	if (myArray != 0) {
		myPixelPointer[0] = b;
		myPixelPointer[1] = g;
		myPixelPointer[2] = r;
		if (myBytesPerPixel == 4) {
			myPixelPointer[3] = 255;
		}
	}
}

void ZLWin32ImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	ZLWin32ImageData &win32source = (ZLWin32ImageData&)source;
	if ((myArray == 0) || (win32source.myArray == 0)) {
		return;
	}
	int height = source.height();
	int bytes = source.width() * myBytesPerPixel;
	BYTE *src = win32source.myArray + win32source.myBytesPerLine * (win32source.myHeight - 1);
	BYTE *dst = myArray + (myHeight - targetY - 1) * myBytesPerLine + targetX * myBytesPerPixel;
	for (int i = 0; i < height; ++i) {
		memcpy(dst, src, bytes);
		dst -= myBytesPerLine;
		src -= win32source.myBytesPerLine;
	}
}

const BYTE *ZLWin32ImageData::pixels(ZLColor bgColor) const {
	if (myArray == 0) {
		return 0;
	}

	if (myBytesPerPixel == 3) {
		return myArray;
	} else {
		if ((myArrayWithoutAlpha == 0) || (myBackgroundColor != bgColor)) {
			myBackgroundColor = bgColor;
			int bpl = (3 * myWidth + 3) >> 2 << 2;
			if (myArrayWithoutAlpha == 0) {
				myArrayWithoutAlpha = new BYTE[bpl * myHeight];	
			}
			const unsigned short R = myBackgroundColor.Red;
			const unsigned short G = myBackgroundColor.Green;
			const unsigned short B = myBackgroundColor.Blue;
			BYTE *from, *to;
			unsigned short b, g, r, a, a2;
			for (unsigned int i = 0; i < myHeight; ++i) {
				from = myArray + myBytesPerLine * i;
				to = myArrayWithoutAlpha + bpl * i;
				for (unsigned int j = 0; j < myWidth; ++j) {
					b = *from++;
					g = *from++;
					r = *from++;
					a = *from++;
					a2 = 255 - a;
					b = b * a + B * a2 + (unsigned short)128;
					g = g * a + G * a2 + (unsigned short)128;
					r = r * a + R * a2 + (unsigned short)128;
					*to++ = (b + (b >> 8)) >> 8;
					*to++ = (g + (g >> 8)) >> 8;
					*to++ = (r + (r >> 8)) >> 8;
				}
			}
		}
		return myArrayWithoutAlpha;
	}
}

const BITMAPINFO &ZLWin32ImageData::info() const {
	return myInfo;
}

shared_ptr<ZLImageData> ZLWin32ImageManager::createData() const {
	return new ZLWin32ImageData();
}

void ZLWin32ImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	ZLWin32ImageData &win32Data = (ZLWin32ImageData&)data;
	if (win32Data.myFailure) {
		return;
	}
	if (pngConvert(stringData, win32Data)) {
		return;
	}
	if (gifConvert(stringData, win32Data)) {
		return;
	}
	if (jpegConvert(stringData, win32Data)) {
		return;
	}
	win32Data.myFailure = true;
}
