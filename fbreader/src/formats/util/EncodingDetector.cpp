/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <enca.h>

#include <ZLInputStream.h>

#include "EncodingDetector.h"

static const int BUFSIZE = 65536;

std::string EncodingDetector::detect(ZLInputStream &stream, Language language) {
	std::string encodingString = "ISO-8859-1";
	if (stream.open()) {
		std::string languageString = "none";
		switch (language) {
			case RUSSIAN:
				languageString = "ru";
				break;
			case CHINESE:
				languageString = "zh";
				break;
			case CZECH:
				languageString = "cs";
				break;
			case OTHER:
				break;
		}
		EncaAnalyser analyser = enca_analyser_alloc(languageString.c_str());
		if (analyser != 0) {
			unsigned char *buffer = new unsigned char[BUFSIZE];
			size_t buflen = stream.read((char*)buffer, BUFSIZE);

			enca_set_filtering(analyser, 0);
			EncaEncoding encoding = enca_analyse(analyser, buffer, buflen);
			const char *e = enca_charset_name(encoding.charset, ENCA_NAME_STYLE_MIME);
			if (e != 0) {
				encodingString = e;
			}
			if (encodingString == "GB2312") {
				encodingString = "GBK";
			}
			enca_analyser_free(analyser);

			delete[] buffer;
		}

		stream.close();
	}

	return encodingString;
}
