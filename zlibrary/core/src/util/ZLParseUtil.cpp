/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLStringUtil.h"
#include "ZLUnicodeUtil.h"

#include "ZLParseUtil.h"


bool ZLParseUtil::parseHTTPStatusLine(const std::string &line, 
		std::string &httpVersion, std::string &statusCode, std::string &reasonPhrase) {
	
	static const std::string PREFIX = "HTTP/";

	if (line.size() < PREFIX.size() || ZLUnicodeUtil::toUpper(line.substr(0, PREFIX.size())) != PREFIX) {
		return false;
	}
	size_t index = PREFIX.size();

	httpVersion.clear();
	statusCode.clear();
	reasonPhrase.clear();

	while (index < line.size() && isdigit(line[index])) {
		httpVersion.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != '.') {
		return false;
	}
	httpVersion.append(1, '.');
	while (index < line.size() && isdigit(line[index])) {
		httpVersion.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != ' ') {
		return false;
	}
	while (index < line.size() && isdigit(line[index])) {
		statusCode.append(1, line[index++]);
	}
	if (index >= line.size() || line[index++] != ' ') {
		return false;
	}
	while (index < line.size() && line[index] != '\r' && line[index] != '\n') {
		reasonPhrase.append(1, line[index++]);
	}
	return true;
}
