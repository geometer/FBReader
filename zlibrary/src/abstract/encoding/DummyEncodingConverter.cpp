/*
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

#include <ZLUnicodeUtil.h>

#include "DummyEncodingConverter.h"

bool DummyEncodingConverterProvider::providesConverter(const std::string &encoding) {
	const std::string lowerCasedEncoding = ZLUnicodeUtil::toLower(encoding);
	return (lowerCasedEncoding == "utf-8") || (lowerCasedEncoding == "us-ascii");
}

shared_ptr<ZLEncodingConverter> DummyEncodingConverterProvider::createConverter(const std::string&) {
	return new DummyEncodingConverter();
}

DummyEncodingConverter::DummyEncodingConverter() {
}

DummyEncodingConverter::~DummyEncodingConverter() {
}

void DummyEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	dst.append(srcStart, srcEnd - srcStart);
}

bool DummyEncodingConverter::fillTable(int *map) {
	for (int i = 0; i < 255; ++i) {
		map[i] = i;
	}
	return true;
}
