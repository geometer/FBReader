/*
 * FBReader -- electronic book reader
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

#ifndef __BINARYIMAGE_H__
#define __BINARYIMAGE_H__

#include <vector>
#include <string>

#include <abstract/ZLImage.h>

class BinaryImage : public ZLSingleImage {

public:
	BinaryImage(const std::string &mimeType) IMAGE_SECTION;
	~BinaryImage() IMAGE_SECTION;
	void addData(const std::string &text);
	const shared_ptr<std::string> stringData() const IMAGE_SECTION;

private:
	mutable shared_ptr<std::string> myData;
};

inline BinaryImage::BinaryImage(const std::string &mimeType) : ZLSingleImage(mimeType) 
{
    myData = new std::string();
}

inline BinaryImage::~BinaryImage() {}

inline void BinaryImage::addData(const std::string &text) {
    myData->append(text);
}
const shared_ptr<std::string> BinaryImage::stringData() const {
	return myData;
}

#endif /* __BINARYIMAGE_H__ */
