/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLInputStream.h>

#include "FormatPlugin.h"

#include "../description/BookDescription.h"

void FormatPlugin::detectEncodingAndLanguage(BookDescription &description, ZLInputStream &stream) {
	std::string encoding = description.encoding();
	if (encoding.empty()) {
		encoding = EncodingDetector::detect(stream, (EncodingDetector::Language)PluginCollection::instance().DefaultLanguageOption.value());
		if (encoding == "unknown") {
			encoding = "windows-1252";
		}
		WritableBookDescription(description).encoding() = encoding;
	}

	if (description.language() == "") {
		if ((encoding == "US-ASCII") ||
				(encoding == "ISO-8859-1")) {
			WritableBookDescription(description).language() = "en";
		} else if ((description.encoding() == "KOI8-R") ||
				(encoding == "windows-1251") ||
				(encoding == "ISO-8859-5") ||
				(encoding == "IBM866")) {
			WritableBookDescription(description).language() = "ru";
		} else if (
			(PluginCollection::instance().DefaultLanguageOption.value() == EncodingDetector::CZECH) &&
			((encoding == "windows-1250") ||
			 (encoding == "ISO-8859-2") ||
			 (encoding == "IBM852"))) {
			WritableBookDescription(description).language() = "cs";
		}
	}
}

void FormatPlugin::defaultTitle(BookDescription &description, const std::string &title) {
	if (description.title().empty()) {
		WritableBookDescription(description).title() = title;
	}
}

const std::string &FormatPlugin::tryOpen(const std::string &path) const {
	static const std::string EMPTY = "";
	return EMPTY;
}
