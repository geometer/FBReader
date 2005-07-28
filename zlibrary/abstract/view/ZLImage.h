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
#include <abstract/shared_ptr.h>

class ZLImage {

protected:
	ZLImage(const std::string &mimeType) IMAGE_SECTION;

public:
	virtual ~ZLImage() IMAGE_SECTION;
	const std::string &mimeType() const IMAGE_SECTION;
	virtual shared_ptr<ZLString> data() const IMAGE_SECTION = 0;

private:
	std::string myMimeType;
};

class ZLBase64EncodedImage : public ZLImage {

public:
	ZLBase64EncodedImage(const std::string &mimeType) IMAGE_SECTION;
	~ZLBase64EncodedImage() IMAGE_SECTION;
	void addData(const ZLStringBuffer &text) IMAGE_SECTION;
	shared_ptr<ZLString> data() const IMAGE_SECTION;

private:
	void decode() const IMAGE_SECTION;

private:
	mutable ZLString myEncodedData;
	mutable shared_ptr<ZLString> myData;
};

class ZLZCompressedFileImage : public ZLImage {

public:
	ZLZCompressedFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t compressedSize) IMAGE_SECTION;
	~ZLZCompressedFileImage() IMAGE_SECTION;
	shared_ptr<ZLString> data() const IMAGE_SECTION;

private:
	std::string myPath;
	size_t myOffset;
	size_t myCompressedSize;
};

inline ZLImage::ZLImage(const std::string &mimeType) : myMimeType(mimeType) {}
inline ZLImage::~ZLImage() {}
inline const std::string &ZLImage::mimeType() const { return myMimeType; }

inline ZLBase64EncodedImage::ZLBase64EncodedImage(const std::string &mimeType) : ZLImage(mimeType) {}
inline ZLBase64EncodedImage::~ZLBase64EncodedImage() {}
inline void ZLBase64EncodedImage::addData(const ZLStringBuffer &text) { myEncodedData += text; }
inline shared_ptr<ZLString> ZLBase64EncodedImage::data() const { decode(); return myData; }

inline ZLZCompressedFileImage::ZLZCompressedFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t compressedSize) : ZLImage(mimeType), myPath(path), myOffset(offset), myCompressedSize(compressedSize) {}
inline ZLZCompressedFileImage::~ZLZCompressedFileImage() {}

#endif /* __ZLIMAGE_H__ */
