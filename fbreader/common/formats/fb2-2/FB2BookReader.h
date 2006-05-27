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

#ifndef __FB2BOOKREADER_H__
#define __FB2BOOKREADER_H__

#include <abstract/ZLXMLReader.h>

#include "../../bookmodel/BookReader.h"

class BookModel;
class Base64EncodedImage;
class FB2BookReader;

class FB2TagAction {

public:
  virtual ~FB2TagAction();
	
  virtual void doAtStart(FB2BookReader &reader, const char **xmlattributes);
  virtual void doAtEnd(FB2BookReader &reader) = 0;
};

class FB2BookReader : public ZLXMLReader {

private:
  static std::map<std::string,FB2TagAction*> ourTagActions;
  static void fillTagTable();

public:
  FB2BookReader(BookModel &model) FORMATS_SECTION;
  ~FB2BookReader() FORMATS_SECTION;
  bool readBook(shared_ptr<ZLInputStream> stream) FORMATS_SECTION;

  void startElementHandler(const char *tag, const char **attributes) FORMATS_SECTION;
  void endElementHandler(const char *tag) FORMATS_SECTION;
  void characterDataHandler(const char *text, int len) FORMATS_SECTION;

private:
  int mySectionDepth;
  int myBodyCounter;
  bool myInsidePoem;
  BookReader myModelReader;

  Base64EncodedImage *myCurrentImage;
  bool myProcessingImage;
  std::vector<std::string> myImageBuffer;

  bool mySectionStarted;
  bool myInsideTitle1;

  friend class FB2TagAction;
  friend class FB2TagControlAction;
  friend class FB2TagParagraphWithControlAction;
};

inline FB2BookReader::~FB2BookReader() {}
inline FB2TagAction::~FB2TagAction() {}

#endif /* __FB2BOOKREADER_H__ */
