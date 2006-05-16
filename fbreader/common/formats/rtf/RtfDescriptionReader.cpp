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

#include <abstract/ZLInputStream.h>

#include "RtfDescriptionReader.h"

RtfDescriptionReader::RtfDescriptionReader(BookDescription &description) : RtfReader(description.encoding()), myDescription(description) {
//    DPRINT("Constructor\n");
  state = READ_NONE;
  hasTitle = false;
  hasAuthor = false;
  hasEncoding = false;
}

void RtfDescriptionReader::startDocumentHandler() {
  state = READ_NONE;
  hasTitle = false;
  hasAuthor = false;
  hasEncoding = false;

  title.erase();
  author.erase();
  encoding.erase();
}

void RtfDescriptionReader::endDocumentHandler()
{
  //DPRINT("End doc handler, title: %s, author: %s, encoding: %s\n", 
  //myDescription.title().data(), author.data(), encoding.data());

  //DPRINT("End doc handler, old encoding: %s\n", 
  //myDescription.encoding().data());

  if (!title.empty()) {
    myDescription.title() = title;
  }
    
  myDescription.addAuthor(author, std::string(), std::string());
    
  if (!encoding.empty()) {
    myDescription.encoding() = encoding;
  }
}

void RtfDescriptionReader::addCharData(const char *data, size_t len, bool convert) {
  // TODO: use parameter 'convert'
  if ((state == READ_TITLE) || (state == READ_AUTHOR)) {
    outputBuffer.append(data, len);
  }
}

void RtfDescriptionReader::flushBuffer() {
  if (!outputBuffer.empty()) {
    std::string newString;
    myConverter->convert(newString, outputBuffer.data(), outputBuffer.data() + outputBuffer.size());
    characterDataHandler(newString);
    outputBuffer.erase();
  }
}

bool RtfDescriptionReader::characterDataHandler(std::string &str) {
  switch (state) {
    case READ_TITLE:
      title.append(str);
      return true;
    case READ_AUTHOR:
      author.append(str);
      return true;
  }
  return false;
}

void RtfDescriptionReader::startElementHandler(int) {
  hasEncoding = true;
}

void RtfDescriptionReader::switchDestination(Destination destiantion, bool on) {
  switch (destiantion) {
    case DESTINATION_INFO:
      flushBuffer();
      if (!on) {
        interrupt();
      }
      break;
    case DESTINATION_TITLE:
      flushBuffer();
      if (on) {
        state = READ_TITLE;
      } else {
        hasTitle = true;
      }
      break;
    case DESTINATION_AUTHOR:
      flushBuffer();
      if (on) {
        state = READ_AUTHOR;
      } else {
        hasAuthor = true;
      }
      break;
    case DESTINATION_NONE:
    case DESTINATION_SKIP:
      break;
    default:
			if (on) {
				state = READ_NONE;
			}
      break;
  }
  if (!on && hasTitle && hasAuthor && hasEncoding) {
		flushBuffer();
    interrupt();
  }
}

void RtfDescriptionReader::insertImage(const std::string&, const std::string&, size_t, size_t) {
}

void RtfDescriptionReader::setFontProperty(FontProperty, bool) {
}

void RtfDescriptionReader::newParagraph() {
}
