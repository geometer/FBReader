/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "AsciiEncoder.h"

static std::string SpecialSymbols = "&<>\"'";

std::string AsciiEncoder::encode(const std::string &source) {
	std::string target;
	target.reserve(4 * source.length());
	for (unsigned int i = 0; i < source.length(); i++) {
		unsigned char num = source[i];
		if (source[i] == '>') {
			target += "&gt;";
		} else if (source[i] == '<') {
			target += "&lt;";
		} else if ((num >= 0x20) && (num < 0x7F) && ((int)SpecialSymbols.find(num) == -1)) {
			if (source[i] == '\\') {
				target += '\\';
			}
			target += source[i];
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
	std::string target;
	target.reserve(source.length());
	for (unsigned int i = 0; i < source.length();) {
		if (source[i] != '\\') {
			target += source[i];
			i++;
		} else {
			i++;
			if (i == source.length()) {
				break;
			}
			if (source[i] == '\\') {
				target += '\\';
				i++;
			} else {
				if (i + 2 >= source.length()) {
					break;
				}
				target += (unsigned char)(100 * (source[i] - '0') + 10 * (source[i + 1] - '0') + source[i + 2] - '0');
				i += 3;
			}
		}
	}
	return target;
}
