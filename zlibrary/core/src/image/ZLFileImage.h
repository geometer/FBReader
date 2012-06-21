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

#ifndef __ZLFILEIMAGE_H__
#define __ZLFILEIMAGE_H__

#include <string>

#include <ZLFile.h>
#include <ZLImage.h>

class ZLFileImage : public ZLSingleImage {

public:
	static const std::string ENCODING_NONE;
	static const std::string ENCODING_HEX;
	static const std::string ENCODING_BASE64;

public:
	ZLFileImage(const ZLFile &file, size_t offset, size_t size = 0, const std::string &encoding = "");
	const shared_ptr<std::string> stringData() const;

protected:
	//shared_ptr<ZLInputStream> inputStream() const;

private:
	const ZLFile myFile;
	const std::string myEncoding;
	const size_t myOffset;
	const size_t mySize;
};

#endif /* __ZLFILEIMAGE_H__ */
