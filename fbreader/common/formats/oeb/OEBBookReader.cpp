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
#include <algorithm>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "OEBBookReader.h"
#include "XHTMLReader.h"
#include "../../bookmodel/BookModel.h"

OEBBookReader::OEBBookReader(BookModel &model) : myModelReader(model) {
}

void OEBBookReader::characterDataHandler(const char*, int) {
}

static const std::string MANIFEST = "manifest";
static const std::string SPINE = "spine";
static const std::string GUIDE = "guide";

static const std::string ITEM = "item";
static const std::string ITEMREF = "itemref";
static const std::string REFERENCE = "reference";

void OEBBookReader::startElementHandler(const char *tag, const char **xmlattributes) {
  if (MANIFEST == tag) {
    myState = READ_MANIFEST;
  } else if (SPINE == tag) {
    myState = READ_SPINE;
  } else if (GUIDE == tag) {
    myState = READ_GUIDE;
  } else if ((myState == READ_MANIFEST) && (ITEM == tag)) {
    const char *id = attributeValue(xmlattributes, "id");
    const char *href = attributeValue(xmlattributes, "href");
    if ((id != 0) && (href != 0)) {
      myIdToHref[id] = href;
    }
  } else if ((myState == READ_SPINE) && (ITEMREF == tag)) {
    const char *id = attributeValue(xmlattributes, "idref");
    if (id != 0) {
      const std::string &fileName = myIdToHref[id];
      if (!fileName.empty()) {
	myHtmlFileNames.push_back(fileName);
      }
    }
  } else if ((myState == READ_GUIDE) && (REFERENCE == tag)) {
    const char *title = attributeValue(xmlattributes, "title");
    const char *href = attributeValue(xmlattributes, "href");
    if ((title != 0) && (href != 0)) {
      myTOC.push_back(std::pair<std::string,std::string>(title, href));
    }
  }
}

void OEBBookReader::endElementHandler(const char *tag) {
  if ((MANIFEST == tag) || (SPINE == tag) || (GUIDE == tag)) {
    myState = READ_NONE;
  }
}

bool OEBBookReader::readBook(const std::string &fileName) {
	std::cerr << "fileName = " << fileName << "\n";
  int index0 = fileName.rfind(':');
  int index1 = fileName.rfind('/');
  myFilePrefix = fileName.substr(0, std::max(index0, index1) + 1);

	std::cerr << "filePrefix = " << myFilePrefix << "\n";
  myIdToHref.clear();
  myHtmlFileNames.clear();
  myTOC.clear();
  myState = READ_NONE;

  if (!readDocument(ZLFile(fileName).inputStream())) {
    return false;
  }

  myModelReader.setMainTextModel();
  myModelReader.pushKind(REGULAR);

  for (std::vector<std::string>::const_iterator it = myHtmlFileNames.begin(); it != myHtmlFileNames.end(); it++) {
    XHTMLReader(myModelReader).readFile(myFilePrefix, *it);
  }

  for (std::vector<std::pair<std::string, std::string> >::const_iterator it = myTOC.begin(); it != myTOC.end(); it++) {
    std::cerr << it->first << " : " << it->second << "\n";
  }

  return true;
}
