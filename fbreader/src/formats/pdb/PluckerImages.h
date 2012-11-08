/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __PLUCKERIMAGES_H__
#define __PLUCKERIMAGES_H__

#include <string>

#include <ZLImage.h>
#include <ZLFile.h>
#include "../../bookmodel/BookModel.h"

class ZCompressedFileImage : public ZLSingleImage {

public:
	ZCompressedFileImage(const ZLFile &file, std::size_t offset, std::size_t size);
	const shared_ptr<std::string> stringData() const;

private:
	const ZLFile myFile;
	const std::size_t myOffset;
	const std::size_t myCompressedSize;
};

class DocCompressedFileImage : public ZLSingleImage {

public:
	DocCompressedFileImage(const ZLFile &file, std::size_t offset, std::size_t compressedSize);
	const shared_ptr<std::string> stringData() const;

private:
	const ZLFile myFile;
	const std::size_t myOffset;
	const std::size_t myCompressedSize;
};

class PluckerMultiImage : public ZLMultiImage {

public:
	PluckerMultiImage(unsigned int rows, unsigned int columns, const ZLImageMap &imageMap);

	void addId(const std::string &id);

	unsigned int rows() const;
	unsigned int columns() const;
	shared_ptr<const ZLImage> subImage(unsigned int row, unsigned int column) const;

private:
	unsigned int myRows, myColumns;
	const ZLImageMap &myImageMap;
	std::vector<std::string> myIds;
};

inline ZCompressedFileImage::ZCompressedFileImage(const ZLFile &file, std::size_t offset, std::size_t compressedSize) : ZLSingleImage(file.mimeType()), myFile(file), myOffset(offset), myCompressedSize(compressedSize) {}

inline DocCompressedFileImage::DocCompressedFileImage(const ZLFile &file, std::size_t offset, std::size_t compressedSize) : ZLSingleImage(file.mimeType()), myFile(file), myOffset(offset), myCompressedSize(compressedSize) {}

inline PluckerMultiImage::PluckerMultiImage(unsigned int rows, unsigned int columns, const ZLImageMap &imageMap) : myRows(rows), myColumns(columns), myImageMap(imageMap) {}
inline void PluckerMultiImage::addId(const std::string &id) { myIds.push_back(id); }
inline unsigned int PluckerMultiImage::rows() const { return myRows; }
inline unsigned int PluckerMultiImage::columns() const { return myColumns; }

#endif /* __PLUCKERIMAGES_H__ */
