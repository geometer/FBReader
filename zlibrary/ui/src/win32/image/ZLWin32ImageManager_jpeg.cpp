/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>

#include "ZLWin32ImageManager.h"

struct JpegSourceManager : public jpeg_source_mgr {
	static void initSourceCallback(j_decompress_ptr info);
	static void termSourceCallback(j_decompress_ptr info);
	static boolean fillBufferCallBack(j_decompress_ptr info);
	static void skipDataCallback(j_decompress_ptr, long len);

	JpegSourceManager(const std::string &data);
	~JpegSourceManager();

	const std::string &myData;
	JOCTET *myBuffer;
	unsigned int myOffset;
};

JpegSourceManager::JpegSourceManager(const std::string &data) : myData(data), myOffset(0) {
  init_source = initSourceCallback;
  fill_input_buffer = fillBufferCallBack;
  skip_input_data = skipDataCallback;
  resync_to_restart = jpeg_resync_to_restart;
  term_source = termSourceCallback;
  bytes_in_buffer = 0;
	myBuffer = new JOCTET[2048];
  next_input_byte = myBuffer;
}

JpegSourceManager::~JpegSourceManager() {
	delete[] myBuffer;
}

void JpegSourceManager::initSourceCallback(j_decompress_ptr) {
}

void JpegSourceManager::termSourceCallback(j_decompress_ptr) {
}

boolean JpegSourceManager::fillBufferCallBack(j_decompress_ptr info) {
	JpegSourceManager &sourceManager = *(JpegSourceManager*)info->src;
	const unsigned int len =
		std::min((unsigned int)2048, sourceManager.myData.length() - sourceManager.myOffset);
	if (len > 0) {
		sourceManager.bytes_in_buffer = len;
		memcpy(sourceManager.myBuffer, sourceManager.myData.data() + sourceManager.myOffset, len);
		sourceManager.myOffset += len;
	} else {
		sourceManager.bytes_in_buffer = 2;
		sourceManager.myBuffer[0] = (JOCTET)0xFF;
		sourceManager.myBuffer[1] = (JOCTET)JPEG_EOI;
	}
	sourceManager.next_input_byte = sourceManager.myBuffer;
	return true;
}

void JpegSourceManager::skipDataCallback(j_decompress_ptr info, long len) {
	JpegSourceManager &sourceManager = *(JpegSourceManager*)info->src;
	if ((long)sourceManager.bytes_in_buffer > len) {
		sourceManager.bytes_in_buffer -= len;
		sourceManager.next_input_byte += len;
	} else {
		sourceManager.myOffset += len - sourceManager.bytes_in_buffer;
		sourceManager.myOffset = std::min(sourceManager.myOffset, sourceManager.myData.length());
		fillBufferCallBack(info);
	}
}

static void errorExit(j_common_ptr info);

struct JpegErrorManager : public jpeg_error_mgr {
  jmp_buf mySetjmpBuffer;
};

static void errorExit(j_common_ptr info) {
	longjmp(((JpegErrorManager*)info->err)->mySetjmpBuffer, 1);
}

bool ZLWin32ImageManager::jpegConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	struct jpeg_decompress_struct info;
	JpegSourceManager reader(stringData);
	JpegErrorManager errorManager;

	jpeg_create_decompress(&info);

	info.src = &reader;
	info.err = jpeg_std_error(&errorManager);
	errorManager.error_exit = errorExit;

	if (setjmp(errorManager.mySetjmpBuffer)) {
		jpeg_destroy_decompress(&info);
		return false;
	}

	jpeg_read_header(&info, true);

	jpeg_start_decompress(&info);

	if ((info.output_components != 1) &&
			(info.output_components != 3) &&
			(info.output_components != 4)) {
		jpeg_destroy_decompress(&info);
		return false;
	}

	data.init(info.output_width, info.output_height, info.output_components == 4, 0);

	unsigned char **rowPointers = new unsigned char*[info.output_height];
	for (unsigned int i = 0; i < info.output_height; ++i) {
		rowPointers[i] = data.myArray + (info.output_height - i - 1) * data.myBytesPerLine;
	}
	while (info.output_scanline < info.output_height) {
		jpeg_read_scanlines(&info, rowPointers + info.output_scanline, info.output_height);
	}
	delete[] rowPointers;

	if (info.output_components == 1) {
		for (unsigned int i = 0; i < data.myHeight; ++i) {
			unsigned char *from = data.myArray + data.myBytesPerLine * i + data.myWidth - 1;
			unsigned char *to = data.myArray + data.myBytesPerLine * i + 3 * (data.myWidth - 1);
			for (unsigned int j = 0; j < data.myWidth; ++j, --from, to -= 3) {
				to[0] = *from;
				to[1] = *from;
				to[2] = *from;
			}
		}
	} else {
		data.bgr2rgb();
	}

	jpeg_finish_decompress(&info);
	jpeg_destroy_decompress(&info);

	return true;
}
