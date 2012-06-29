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

#include <ZLInputStream.h>
#include <ZLLogger.h>

#include "ZLBlockedFileImage.h"

ZLBlockedFileImage::ZLBlockedFileImage(const ZLFile &file, const Blocks &blocks) :
	ZLSingleImage(file.mimeType()),
	myFile(file),
	myBlocks(blocks) {
}

const shared_ptr<std::string> ZLBlockedFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = myFile.inputStream();
	if (stream.isNull() || !stream->open()) {
		return 0;
	}

	shared_ptr<std::string> imageData = new std::string();
	for (size_t i = 0; i < myBlocks.size(); ++i) {
		Block block = myBlocks.at(i);
		stream->seek(block.offset, true);
		char* buffer = new char[block.size];
		size_t readed = stream->read(buffer, block.size);
		if (readed != block.size) {
			ZLLogger::Instance().println("ZLBlockedFileImage", "stringData(), not all bytes readed");
		}
		imageData->append(buffer, readed);
		delete[] buffer;
	}
	return imageData;
}
