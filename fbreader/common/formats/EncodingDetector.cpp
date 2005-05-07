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

#ifndef PALM_TEMPORARY

#include <enca.h>

#endif // PALM_TEMPORARY

#include <abstract/ZLInputStream.h>

#include "EncodingDetector.h"

static const int BUFSIZE = 50120;

std::string EncodingDetector::detect(ZLInputStream &stream) {
	if (!stream.open()) {
		return "";
	}

#ifndef PALM_TEMPORARY

	unsigned char *buffer = new unsigned char[BUFSIZE];

	size_t buflen = stream.read((char*)buffer, BUFSIZE);
	EncaAnalyser analyser = enca_analyser_alloc("ru");
  enca_set_filtering(analyser, 0);
	EncaEncoding encoding = enca_analyse_const(analyser, buffer, buflen);
	const char *e = enca_charset_name(encoding.charset, ENCA_NAME_STYLE_MIME);
	std::string encodingString = (e != 0) ? e : "unknown";
	enca_analyser_free(analyser);

	delete[] buffer;
	stream.close();

	return (encodingString == "unknown") ? "windows-1252" : encodingString;

#else // PALM_TEMPORARY

	return "windows-1252";

#endif // PALM_TEMPORARY
}
