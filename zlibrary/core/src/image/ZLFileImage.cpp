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

#include <ZLFile.h>
#include <ZLLogger.h>
#include <ZLInputStream.h>
#include <ZLSliceInputStream.h>
#include <ZLBase64InputStream.h>
#include <ZLHexInputStream.h>

//#include <ZLHexEncodedImage.h>
//#include <ZLBase64EncodedImage.h>

#include "ZLFileImage.h"

const std::string ZLFileImage::ENCODING_NONE = "";
const std::string ZLFileImage::ENCODING_HEX = "hex";
const std::string ZLFileImage::ENCODING_BASE64 = "base64";

ZLFileImage::ZLFileImage(const ZLFile &file, size_t offset, size_t size, const std::string &encoding) :
	ZLSingleImage(file.mimeType()),
	myFile(file),
	myEncoding(encoding),
	myOffset(offset),
	mySize(size) {
}

const shared_ptr<std::string> ZLFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = inputStream();
	if (stream.isNull() || !stream->open()) {
		return 0;
	}
	size_t size = mySize;
	if (size == 0) {
		size = stream->sizeOfOpened();
		if (size == 0) {
			return 0;
		}
	}

	shared_ptr<std::string> imageData = new std::string();

	char *buffer = new char[size];
	size_t readed = stream->read(buffer, size);
	if (readed != size) {
		ZLLogger::Instance().println("ZLFileImage", "stringData(), not all bytes readed");
	}
	imageData->append(buffer, readed);
	delete[] buffer;

	//old code for not-stream reading:
//	shared_ptr<std::string> imageData = new std::string();
//	stream->seek(myOffset, true);
//	char *buffer = new char[size];
//	size_t readed = stream->read(buffer, size);
//	if (readed != size) {
//		ZLLogger::Instance().println("ZLFileImage", "stringData(), not all bytes readed");
//	}
//	imageData->append(buffer, readed);
//	delete[] buffer;

//	if (myEncoding == ENCODING_HEX) {
//		ZLHexEncodedImage image(mimeType(), imageData);
//		return image.stringData();
//	} else if (myEncoding == ENCODING_BASE64) {
//		ZLBase64EncodedImage image(mimeType());
//		image.addData(*imageData, 0, std::string::npos);
//		return image.stringData();
//	}

	return imageData;
}


shared_ptr<ZLInputStream> ZLFileImage::inputStream() const {
	shared_ptr<ZLInputStream> stream = new ZLSliceInputStream(myFile.inputStream(), myOffset, mySize);
	if (myEncoding == ENCODING_HEX) {
		return new ZLHexInputStream(stream);
	} else if (myEncoding == ENCODING_BASE64) {
		return new ZLBase64InputStream(stream);
	}
	if (myEncoding != ENCODING_NONE) {
		ZLLogger::Instance().println("ZLFileImage", "unsupported encoding: " + myEncoding);
	}
	return stream;
}
