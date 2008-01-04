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

#include <ZLInputStream.h>
#include <ZLLanguageDetector.h>

#include "FormatPlugin.h"

#include "../description/BookDescription.h"

void FormatPlugin::detectEncodingAndLanguage(BookDescription &description, ZLInputStream &stream) {
	std::string language = description.language();
	std::string encoding = description.encoding();
	if (encoding.empty() || language.empty()) {
		PluginCollection &collection = PluginCollection::instance();
		if (language.empty()) {
			language = collection.DefaultLanguageOption.value();
		}
		if (encoding.empty()) {
			encoding = collection.DefaultEncodingOption.value();
		}
		if (collection.LanguageAutoDetectOption.value() && stream.open()) {
			static const int BUFSIZE = 65536;
			char *buffer = new char[BUFSIZE];
			const size_t size = stream.read(buffer, BUFSIZE);
			stream.close();
			shared_ptr<ZLLanguageDetector::LanguageInfo> info =
				ZLLanguageDetector().findInfo(buffer, size);
			delete[] buffer;
			if (!info.isNull()) {
				language = info->Language;
				encoding = info->Encoding;
				if ((encoding == "US-ASCII") || (encoding == "ISO-8859-1")) {
					encoding = "windows-1252";
				}
			}
		}
		WritableBookDescription(description).encoding() = encoding;
		WritableBookDescription(description).language() = language;
	}
}

const std::string &FormatPlugin::tryOpen(const std::string &path) const {
	static const std::string EMPTY = "";
	return EMPTY;
}
