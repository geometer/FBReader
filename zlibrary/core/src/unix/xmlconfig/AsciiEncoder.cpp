/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include "AsciiEncoder.h"

std::string AsciiEncoder::encode(const std::string &source) {
	const char *start = source.data();
	const char *end = start + source.length();

	bool doEncode = false;
	for (const char *ptr = start; ptr < end; ++ptr) {
		if ((*ptr == '&') || (*ptr == '<') || (*ptr == '>') ||
				(*ptr == '"') || (*ptr == '\'') || (*ptr == '\\')) {
			doEncode = true;
			break;
		}
	}

	if (!doEncode && ZLUnicodeUtil::isUtf8String(source)) {
		return source;
	}

	std::string target;
	target.reserve(6 * source.length());
	for (const char *ptr = start; ptr < end; ++ptr) {
		const unsigned char num = *ptr;
		if (num == '>') {
			target += "&gt;";
		} else if (num == '<') {
			target += "&lt;";
		} else if (num == '&') {
			target += "&amp;";
		} else if (num == '\'') {
			target += "&apos;";
		} else if (num == '"') {
			target += "&quot;";
		} else if (num == '\\') {
			target += "\\\\";
		} else if (num < 0x7F) {
			target += num;
		} else {
			target += '\\';
			target += '0' + num / 100;
			target += '0' + num / 10 % 10;
			target += '0' + num % 10;
		}
	}
	return target;
}

std::string AsciiEncoder::decode(const std::string &source) {
	if ((int)source.find('\\') == -1) {
		return source;
	}

	const char *start = source.data();
	const char *end = start + source.length();
	std::string target;
	target.reserve(source.length());
	for (const char *ptr = start; ptr < end;) {
		if (*ptr != '\\') {
			target += *ptr;
			++ptr;
		} else {
			++ptr;
			if (ptr == end) {
				break;
			}
			if (*ptr == '\\') {
				target += '\\';
				++ptr;
			} else {
				if (ptr + 2 >= end) {
					break;
				}
				target += (unsigned char)(100 * (*ptr - '0') + 10 * (*(ptr + 1) - '0') + *(ptr + 2) - '0');
				ptr += 3;
			}
		}
	}
	return target;
}
