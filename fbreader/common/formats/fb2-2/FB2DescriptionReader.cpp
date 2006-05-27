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

#include "FB2DescriptionReader.h"

FB2DescriptionReader::FB2DescriptionReader(BookDescription &description) : myDescription(description) {
}

void FB2DescriptionReader::characterDataHandler(const char *text, int len) {
  switch (myReadState) {
    case READ_TITLE:
      myDescription.title().append(text, len);
      break;
    case READ_LANGUAGE:
      myDescription.language().append(text, len);
      break;
    case READ_AUTHOR_FIRST_NAME:
      myAuthorFirstName.append(text, len);
      break;
    case READ_AUTHOR_MIDDLE_NAME:
      myAuthorMiddleName.append(text, len);
      break;
    case READ_AUTHOR_LAST_NAME:
      myAuthorLastName.append(text, len);
      break;
    default:
      break;
  }
}

static const std::string BODY = "body";
static const std::string TITLE_INFO = "title-info";
static const std::string BOOK_TITLE = "book-title";
static const std::string AUTHOR = "author";
static const std::string LANG = "lang";
static const std::string FIRST_NAME = "first-name";
static const std::string MIDDLE_NAME = "middle-name";
static const std::string LAST_NAME = "last-name";

void FB2DescriptionReader::startElementHandler(const char *tag, const char **) {
  // TODO: tag -> lowercase
  switch (myReadState) {
    case READ_NONE:
      if (BODY == tag) {
        interrupt();
      } else if (TITLE_INFO == tag) {
        myReadState = READ_TITLE_INFO;
      }
      break;
    case READ_TITLE_INFO:
      if (BOOK_TITLE == tag) {
        myReadState = READ_TITLE;
      } else if (AUTHOR == tag) {
        myReadState = READ_AUTHOR;
      } else if (LANG == tag) {
        myReadState = READ_LANGUAGE;
      }
      break;
    case READ_AUTHOR:
      if (FIRST_NAME == tag) {
        myReadState = READ_AUTHOR_FIRST_NAME;
      } else if (MIDDLE_NAME == tag) {
        myReadState = READ_AUTHOR_MIDDLE_NAME;
      } else if (LAST_NAME == tag) {
        myReadState = READ_AUTHOR_LAST_NAME;
      }
      break;
    default:
      break;
  }
}

void FB2DescriptionReader::endElementHandler(const char *tag) {
  if (myReadState == READ_NONE) {
    return;
  }
  // TODO: tag -> lowercase
  if (TITLE_INFO == tag) {
    interrupt();
  } else if ((myReadState == READ_AUTHOR) && (AUTHOR == tag)) {
    myDescription.addAuthor(myAuthorFirstName, myAuthorMiddleName, myAuthorLastName);
    myAuthorFirstName.erase();
    myAuthorMiddleName.erase();
    myAuthorLastName.erase();
    myReadState = READ_TITLE_INFO;
  } else if ((BOOK_TITLE == tag) || (LANG == tag)) {
    myReadState = READ_TITLE_INFO;
  } else if ((FIRST_NAME == tag) || (MIDDLE_NAME == tag) || (LAST_NAME == tag)) {
    myReadState = READ_AUTHOR;
  }
}

bool FB2DescriptionReader::readDescription(shared_ptr<ZLInputStream> stream) {
  myReadState = READ_NONE;
  myAuthorFirstName.erase();
  myAuthorMiddleName.erase();
  myAuthorLastName.erase();
  return readDocument(stream);
}
