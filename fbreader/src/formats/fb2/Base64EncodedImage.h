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

#ifndef __BASE64ENCODEDIMAGE_H__
#define __BASE64ENCODEDIMAGE_H__

#include <vector>

#include <ZLImage.h>

class Base64EncodedImage : public ZLSingleImage {

public:
	Base64EncodedImage(const std::string &mimeType);
	~Base64EncodedImage();
	void addData(const std::vector<std::string> &text);
	const shared_ptr<std::string> stringData() const;

private:
	void decode() const;

private:
	mutable std::string myEncodedData;
	mutable shared_ptr<std::string> myData;
};

inline Base64EncodedImage::Base64EncodedImage(const std::string &mimeType) : ZLSingleImage(mimeType) {}
inline Base64EncodedImage::~Base64EncodedImage() {}

#endif /* __BASE64ENCODEDIMAGE_H__ */
