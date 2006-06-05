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

#ifndef __FB2DESCRIPTIONREADER_H__
#define __FB2DESCRIPTIONREADER_H__

#include <string>

#include <abstract/ZLXMLReader.h>

#include "../../description/BookDescription.h"

class FB2DescriptionReader : public ZLXMLReader {

public:
  FB2DescriptionReader(BookDescription &description);
  ~FB2DescriptionReader();
  bool readDescription(shared_ptr<ZLInputStream> stream);

  void startElementHandler(const char *tag, const char **attributes);
  void endElementHandler(const char *tag);
  void characterDataHandler(const char *text, int len);

private:
  WritableBookDescription myDescription;

  enum {
    READ_NONE,
    READ_TITLE_INFO,
    READ_TITLE,
    READ_AUTHOR,
    READ_AUTHOR_FIRST_NAME,
    READ_AUTHOR_MIDDLE_NAME,
    READ_AUTHOR_LAST_NAME,
    READ_LANGUAGE
  } myReadState;

  std::string myAuthorFirstName;
  std::string myAuthorMiddleName;
  std::string myAuthorLastName;
};

inline FB2DescriptionReader::~FB2DescriptionReader() {}

#endif /* __FB2DESCRIPTIONREADER_H__ */
