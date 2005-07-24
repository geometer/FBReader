/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLIMAGE_H__
#define __ZLIMAGE_H__

#include <abstract/ZLString.h>

class ZLImage {

public:
	ZLImage(const std::string &mimeType) IMAGE_SECTION;
	~ZLImage() IMAGE_SECTION;
	void addData(const ZLStringBuffer &text) IMAGE_SECTION;
	const unsigned char *data() const IMAGE_SECTION;
	unsigned int datalen() const IMAGE_SECTION;
	const std::string &mimeType() const IMAGE_SECTION;

private:
	void decode() const IMAGE_SECTION;

private:
	std::string myMimeType;
	mutable ZLString myEncodedData;
	mutable unsigned char *myData;
	mutable unsigned int myDataLen;
};

inline void ZLImage::addData(const ZLStringBuffer &text) { myEncodedData += text; }
inline const unsigned char *ZLImage::data() const { decode(); return myData; }
inline unsigned int ZLImage::datalen() const { decode(); return myDataLen; }
inline const std::string &ZLImage::mimeType() const { return myMimeType; }

#endif /* __ZLIMAGE_H__ */
