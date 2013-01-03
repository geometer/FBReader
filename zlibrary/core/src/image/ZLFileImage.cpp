/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>
#include <ZLLogger.h>
#include <ZLInputStream.h>

//#include <ZLSliceInputStream.h>
//#include <ZLBase64InputStream.h>
//#include <ZLHexInputStream.h>

#include "ZLHexEncodedImage.h"
#include "ZLBase64EncodedImage.h"

#include "ZLFileImage.h"

const std::string ZLFileImage::ENCODING_NONE = "";
const std::string ZLFileImage::ENCODING_HEX = "hex";
const std::string ZLFileImage::ENCODING_BASE64 = "base64";

ZLFileImage::ZLFileImage(const ZLFile &file, std::size_t offset, std::size_t size, const std::string &encoding) :
	ZLSingleImage(file.mimeType()),
	myFile(file),
	myEncoding(encoding) {
	myBlocks.push_back(Block(offset, size));
}

ZLFileImage::ZLFileImage(const ZLFile &file, const ZLFileImage::Blocks &blocks, const std::string &encoding) :
	ZLSingleImage(file.mimeType()),
	myFile(file),
	myEncoding(encoding),
	myBlocks(blocks) {
}

const shared_ptr<std::string> ZLFileImage::stringData() const {

//code for using streams for images
//	shared_ptr<ZLInputStream> stream = inputStream();
//	if (stream.isNull() || !stream->open()) {
//		return 0;
//	}
//	std::size_t size = mySize;
//	if (size == 0) {
//		size = stream->sizeOfOpened();
//		if (size == 0) {
//			return 0;
//		}
//	}

//	shared_ptr<std::string> imageData = new std::string();

//	char *buffer = new char[size];
//	std::size_t readed = stream->read(buffer, size);
//	if (readed != size) {
//		ZLLogger::Instance().println("ZLFileImage", "stringData(), not all bytes readed");
//	}
//	imageData->append(buffer, readed);
//	delete[] buffer;

//	return imageData;

	shared_ptr<ZLInputStream> stream = myFile.inputStream();
	if (stream.isNull() || !stream->open()) {
		return 0;
	}

	shared_ptr<std::string> imageData = new std::string();
	for (std::size_t i = 0; i < myBlocks.size(); ++i) {
		Block block = myBlocks.at(i);
		stream->seek(block.offset, true);
		std::size_t size = block.size;
		if (size == 0) {
			if (myBlocks.size() != 1) {
				continue;
			} else {
				size = stream->sizeOfOpened();
				if (size == 0) {
					return 0;
				}
			}
		}
		char* buffer = new char[size];
		std::size_t readed = stream->read(buffer, size);
		if (readed != size) {
			ZLLogger::Instance().println("ZLFileImage", "stringData(), not all bytes readed");
		}
		imageData->append(buffer, readed);
		delete[] buffer;
	}

	if (imageData->empty()) {
		return 0;
	}

	if (myEncoding == ENCODING_HEX) {
		ZLHexEncodedImage image(mimeType(), imageData);
		return image.stringData();
	} else if (myEncoding == ENCODING_BASE64) {
		ZLBase64EncodedImage image(mimeType());
		image.addData(*imageData, 0, std::string::npos);
		return image.stringData();
	}

	return imageData;
}


//shared_ptr<ZLInputStream> ZLFileImage::inputStream() const {
//	shared_ptr<ZLInputStream> stream = new ZLSliceInputStream(myFile.inputStream(), myOffset, mySize);
//	if (myEncoding == ENCODING_HEX) {
//		return new ZLHexInputStream(stream);
//	} else if (myEncoding == ENCODING_BASE64) {
//		return new ZLBase64InputStream(stream);
//	}
//	if (myEncoding != ENCODING_NONE) {
//		ZLLogger::Instance().println("ZLFileImage", "unsupported encoding: " + myEncoding);
//	}
//	return stream;
//}
