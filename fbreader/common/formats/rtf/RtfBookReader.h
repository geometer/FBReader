/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 * Copyright (C) 2006 Vladimir Sorokin
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

#ifndef __RTFBOOKREADER_H__
#define __RTFBOOKREADER_H__

#include <vector>

#include "RtfReader.h"
#include "../../bookmodel/BookReader.h"

class BookModel;
class RtfImage;


class RtfBookReader : public RtfReader {

public:
  RtfBookReader(BookModel &model, const std::string &encoding);
  ~RtfBookReader();

  bool readDocument(const std::string &fileName);

  bool characterDataHandler(std::string &str);
  void flushBuffer();

	void setEncoding(int code);
  void setAlignment(AlignmentType alignment);
  void switchDestination(DestinationType destination, bool on);
  void addCharData(const char *data, size_t len, bool convert);
  void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size);

  void setFontProperty(FontProperty property);
  void newParagraph();

private:
  int imageIndex;
  int footnoteIndex;

  struct RtfBookReaderState {
    std::string id;
    bool readText;
    
    bool isPrevImage;
  };

  std::vector<RtfBookReaderState> stack;
  RtfBookReaderState state;

  std::string outputBuffer;

private:
  BookReader myBookReader;
};

inline RtfBookReader::~RtfBookReader() {}

#endif /* __RTFBOOKREADER_H__ */
