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

#ifndef __ZLFILEIMAGE_H__
#define __ZLFILEIMAGE_H__

#include <string>

#include <ZLImage.h>

class ZLFileImage : public ZLSingleImage {

public:
	ZLFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t size = 0);
	~ZLFileImage();
	const shared_ptr<std::string> stringData() const;

private:
	std::string myPath;
	size_t myOffset;
	mutable size_t mySize;
};

inline ZLFileImage::ZLFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t size) : ZLSingleImage(mimeType), myPath(path), myOffset(offset), mySize(size) {}
inline ZLFileImage::~ZLFileImage() {}

#endif /* __ZLFILEIMAGE_H__ */
