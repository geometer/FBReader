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

#include <iostream>

#include <abstract/ZLInputStream.h>

#include "OEBBookReader.h"
#include "../../bookmodel/BookModel.h"

OEBBookReader::OEBBookReader(BookModel &model) : myModelReader(model) {
}

void OEBBookReader::characterDataHandler(const char *text, int len) {
}

static const std::string REFERENCE = "reference";

void OEBBookReader::startElementHandler(const char *tag, const char **xmlattributes) {
  if (REFERENCE == tag) {
    const char *text = attributeValue(xmlattributes, "title");
    if (text != 0) {
      myModelReader.beginContentsParagraph();
      myModelReader.beginParagraph();
      myModelReader.addDataToBuffer(text);
      myModelReader.endParagraph();
      myModelReader.endContentsParagraph();
    }
  } else {
    std::cerr << tag << "\n";
  }
}

void OEBBookReader::endElementHandler(const char *tag) {
}

bool OEBBookReader::readBook(shared_ptr<ZLInputStream> stream) {
  	myModelReader.setMainTextModel();
	myModelReader.pushKind(REGULAR);
	return readDocument(stream);
}
