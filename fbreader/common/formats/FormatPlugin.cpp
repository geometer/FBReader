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

ZLIntegerOption FormatPlugin::DefaultLanguageOption("Format", "DefaultLanguage", EncodingDetector::RUSSIAN);

void FormatPlugin::detectEncoding(BookDescription &description, ZLInputStream &stream) const {
  if (description.encoding().empty()) {
    WritableBookDescription(description).encoding() = EncodingDetector::detect(stream, (EncodingDetector::Language)DefaultLanguageOption.value());
  }
}

void FormatPlugin::defaultLanguage(BookDescription &description) const {
  if (description.language() == "") {
    if ((description.encoding() == "US-ASCII") ||
        (description.encoding() == "ISO-8859-1")) {
      WritableBookDescription(description).language() = "en";
    } else if ((description.encoding() == "KOI8-R") ||
        (description.encoding() == "windows-1251") ||
        (description.encoding() == "ISO-8859-5") ||
        (description.encoding() == "IBM866")) {
      WritableBookDescription(description).language() = "ru";
    }
  }
}

void FormatPlugin::defaultTitle(BookDescription &description, const std::string &title) const {
  if (description.title().empty()) {
    WritableBookDescription(description).title() = title;
  }
}
