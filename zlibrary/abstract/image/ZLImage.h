/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLIMAGE_H__
#define __ZLIMAGE_H__

#include <vector>
#include <map>

#include <abstract/ZLString.h>
#include <abstract/shared_ptr.h>

class ZLImage {

protected:
	ZLImage() IMAGE_SECTION;

public:
	virtual ~ZLImage() IMAGE_SECTION;
	virtual bool isSingle() const = 0;
};

class ZLSingleImage : public ZLImage {

protected:
	ZLSingleImage(const std::string &mimeType) IMAGE_SECTION;
	virtual ~ZLSingleImage() IMAGE_SECTION;

public:
	bool isSingle() const { return true; }
	const std::string &mimeType() const IMAGE_SECTION;
	virtual const shared_ptr<ZLString> stringData() const IMAGE_SECTION = 0;
	
private:
	std::string myMimeType;
};

class ZLBase64EncodedImage : public ZLSingleImage {

public:
	ZLBase64EncodedImage(const std::string &mimeType) IMAGE_SECTION;
	~ZLBase64EncodedImage() IMAGE_SECTION;
	void addData(const ZLStringBuffer &text) IMAGE_SECTION;
	const shared_ptr<ZLString> stringData() const IMAGE_SECTION;

private:
	void decode() const IMAGE_SECTION;

private:
	mutable ZLString myEncodedData;
	mutable shared_ptr<ZLString> myData;
};

class ZLZCompressedFileImage : public ZLSingleImage {

public:
	ZLZCompressedFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t compressedSize) IMAGE_SECTION;
	~ZLZCompressedFileImage() IMAGE_SECTION;
	const shared_ptr<ZLString> stringData() const IMAGE_SECTION;

private:
	std::string myPath;
	size_t myOffset;
	size_t myCompressedSize;
};

class ZLMultiImage : public ZLImage {

protected:
	ZLMultiImage() IMAGE_SECTION;
	virtual ~ZLMultiImage() IMAGE_SECTION;

public:
	bool isSingle() const { return false; }
	virtual unsigned int rows() const = 0;
	virtual unsigned int columns() const = 0;
	virtual const ZLImage *subImage(unsigned int row, unsigned int column) const = 0;
};

inline ZLImage::ZLImage() {}
inline ZLImage::~ZLImage() {}

inline ZLSingleImage::ZLSingleImage(const std::string &mimeType) : myMimeType(mimeType) {}
inline ZLSingleImage::~ZLSingleImage() {}
inline const std::string &ZLSingleImage::mimeType() const { return myMimeType; }

inline ZLMultiImage::ZLMultiImage() : ZLImage() {}
inline ZLMultiImage::~ZLMultiImage() {}

inline ZLBase64EncodedImage::ZLBase64EncodedImage(const std::string &mimeType) : ZLSingleImage(mimeType) {}
inline ZLBase64EncodedImage::~ZLBase64EncodedImage() {}
inline void ZLBase64EncodedImage::addData(const ZLStringBuffer &text) { myEncodedData += text; }

inline ZLZCompressedFileImage::ZLZCompressedFileImage(const std::string &mimeType, const std::string &path, size_t offset, size_t compressedSize) : ZLSingleImage(mimeType), myPath(path), myOffset(offset), myCompressedSize(compressedSize) {}
inline ZLZCompressedFileImage::~ZLZCompressedFileImage() {}

#endif /* __ZLIMAGE_H__ */
