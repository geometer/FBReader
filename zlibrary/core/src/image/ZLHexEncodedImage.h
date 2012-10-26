/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLHEXENCODEDIMAGE_H__
#define __ZLHEXENCODEDIMAGE_H__

#include <vector>

#include <ZLImage.h>

class ZLHexEncodedImage : public ZLSingleImage {

public:
	ZLHexEncodedImage(shared_ptr<ZLMimeType> mimeType, shared_ptr<std::string> encodedData);
	const shared_ptr<std::string> stringData() const;

private:
	void read() const;

private:
	shared_ptr<std::string> myEncodedData;
	mutable shared_ptr<std::string> myData;
};

#endif /* __ZLHEXENCODEDIMAGE_H__ */
