/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <stdio.h>
#include <jpeglib.h>

#include "ZLWin32ImageManager.h"

class Reader2 : public jpeg_source_mgr {

public:
	Reader2(const std::string &data) : myData(data), myOffset(0) {
	}
	/*
	bool read(png_bytep buffer, png_size_t length) {
		if (myOffset + length > myData.length()) {
			return false;
		}
		memcpy(buffer, myData.data() + myOffset, length);
		myOffset += length;
		return true;
	}
	*/

private:
	const std::string &myData;
	size_t myOffset;
};

/*
static void pngReadFunction(png_structp png_ptr, png_bytep data, png_size_t length) {
	Reader2 &reader = *(Reader2*)png_get_io_ptr(png_ptr);
	if (!reader.read(data, length)) {
		png_error(png_ptr, "Read Error");
	}
}
*/

bool ZLWin32ImageManager::jpegConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	struct jpeg_decompress_struct info;
	Reader2 reader(stringData);
	struct jpeg_error_mgr errorManager;

		/*
		jpeg_create_decompress(&cinfo);

		cinfo.src = iod_src;
		cinfo.err = jpeg_std_error(&jerr);
		jerr.error_exit = my_error_exit;

		if (!setjmp(jerr.setjmp_buffer)) {
#if defined(_OS_UNIXWARE7_)
	(void) jpeg_read_header(&cinfo, B_TRUE);
#else
	(void) jpeg_read_header(&cinfo, TRUE);
#endif
	QString params = iio->parameters();
	params.simplifyWhiteSpace();

	if (params.contains("Fast")) {
			cinfo.dct_method = JDCT_IFAST;
			cinfo.do_fancy_upsampling = FALSE;
	}

	//
	// "Shrink" is a scaling factor.	By combining Shrink with
	// Scale, it's possible to quickly load an image and smooth
	// scale it.	The result is a lower quality image but generated
	// in less time.
	//
	if (params.contains("Shrink")) {
			sscanf(params.latin1() + params.find("Shrink"),
		"Shrink( %i )", &cinfo.scale_denom);

			if (cinfo.scale_denom < 2) {
		cinfo.scale_denom = 1;
			} else if (cinfo.scale_denom < 4) {
		cinfo.scale_denom = 2;
			} else if (cinfo.scale_denom < 8) {
		cinfo.scale_denom = 4;
			} else {
		cinfo.scale_denom = 8;
			}
	}

	(void) jpeg_start_decompress(&cinfo);

	if (params.contains("GetHeaderInformation")) {
			image = qJpegHeaderInformation(&cinfo);
	} else {
#ifndef QT_NO_IMAGE_SMOOTHSCALE
			if (params.contains("Scale")) {
		jpegSmoothScaler scaler(&cinfo,
				params.latin1() + params.find("Scale"));
		image = scaler.scale();
			} else
#endif
			{
		if (cinfo.output_components == 3 ||
			cinfo.output_components == 4) {
				image.create(cinfo.output_width, cinfo.output_height, 32);
		} else if (cinfo.output_components == 1) {
				image.create(cinfo.output_width, cinfo.output_height, 8, 256);
				for (int i = 0; i < 256; i++) {
			image.setColor(i, qRgb(i, i, i));
				}
		} else {
				// Unsupported format.
		}

		if (!image.isNull()) {
				uchar **lines = image.jumpTable();
				while (cinfo.output_scanline < cinfo.output_height) {
			(void) jpeg_read_scanlines(&cinfo,
					lines + cinfo.output_scanline, cinfo.output_height);
				}

				if ( cinfo.output_components == 3 ) {
			// Expand 24->32 bpp.
			for (uint j=0; j<cinfo.output_height; j++) {
					uchar *in = image.scanLine(j) +
				cinfo.output_width * 3;
					QRgb *out = (QRgb*)image.scanLine(j);

					for (uint i=cinfo.output_width; i--; ) {
				in-=3;
				out[i] = qRgb(in[0], in[1], in[2]);
					}
			}
				}
		}
			}
			(void) jpeg_finish_decompress(&cinfo);
	}
	iio->setImage(image);
	iio->setStatus(0);
		}

		jpeg_destroy_decompress(&cinfo);
		delete iod_src;
	*/
}
