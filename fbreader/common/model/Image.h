/*
 * FBReader -- electronic book reader
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

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <abstract/ZLStringUtil.h>

class Image {

public:
	Image(const std::string &mimeType) MODEL_SECTION;
	~Image() MODEL_SECTION;
	void addData(const std::vector<std::string> &text) { ZLStringUtil::append(myEncodedData, text); }
	const unsigned char *data() const { decode(); return myData; }
	unsigned int datalen() const { decode(); return myDataLen; }
	const std::string &mimeType() const { return myMimeType; }

private:
	void decode() const MODEL_SECTION;

private:
	std::string myMimeType;
	mutable std::string myEncodedData;
	mutable unsigned char *myData;
	mutable unsigned int myDataLen;
};

#endif /* __IMAGE_H__ */
