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
#include <cctype>

#include <abstract/ZLStringUtil.h>

#include "RtfBookReader.h"
#include "../../bookmodel/BookModel.h"
#include "RtfImage.h"

RtfBookReader::RtfBookReader(BookModel &model, const std::string &encoding) : RtfReader(encoding), myBookReader(model) {
}

static const size_t maxBufferSize = 1024;

void RtfBookReader::addCharData(const char *data, size_t len, bool convert) {
  if (state.readState == READ_TEXT) {
		if (convert || myConverter.isNull()) {
      outputBuffer.append(data, len);
			if (outputBuffer.size() >= maxBufferSize) {
				flushBuffer();
			}
		} else {
			flushBuffer();
      std::string newString(data, len);
      characterDataHandler(newString);
		}
  }
}

void RtfBookReader::flushBuffer() {
  if (!outputBuffer.empty()) {
    if (state.readState == READ_TEXT) {    
			if (!myConverter.isNull()) {
        static std::string newString;
     	 	myConverter->convert(newString, outputBuffer.data(), outputBuffer.data() + outputBuffer.length());
        characterDataHandler(newString);
				newString.erase();
			} else {
        characterDataHandler(outputBuffer);
			}
    }
    outputBuffer.erase();
	}
}

void RtfBookReader::insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size) {
  ZLImage *image = new RtfImage(mimeType, fileName, startOffset, size);

  std::string id = "InternalImage";
	ZLStringUtil::appendNumber(id, imageIndex++);
  myBookReader.addImageReference(id);   
  myBookReader.addImage(id, image);
}

bool RtfBookReader::characterDataHandler(std::string &str) {
  if (state.readState == READ_TEXT) {
    if (state.isPrevImage) {
      myBookReader.beginParagraph();
      state.isPrevImage = false;
    }
    myBookReader.addData(str);
  }
  return true;
}

void RtfBookReader::startDocumentHandler() {
  imageIndex = 0;
  footnoteIndex = 1;

  currentStyleInfo = 0;    
  
  state.readState = READ_NONE;
  state.isItalic = false;
  state.isBold = false;
  state.id = "";
  state.style = -1;
  state.isPrevImage = false;

  myBookReader.setMainTextModel();
  myBookReader.pushKind(REGULAR);
  myBookReader.beginParagraph();
}

void RtfBookReader::endDocumentHandler() {
  flushBuffer();
  myBookReader.endParagraph();
}

void RtfBookReader::startElementHandler(int tag) {
  switch(tag) {
    case _P:
      flushBuffer();
      myBookReader.endParagraph();
      myBookReader.beginParagraph();
      break;
    case _BOLD:
      if (state.readState != READ_TEXT) {
        DPRINT("change style not in text.\n");
        break;
      }
        
      flushBuffer();
        
      state.isBold = true;
        
      DPRINT("add style strong.\n");
        
      myBookReader.pushKind(STRONG);
      myBookReader.addControl(STRONG, true);

      break;        
    case _ITALIC:
      if (state.readState != READ_TEXT) {
        DPRINT("change style not in text.\n");
        break;
      }
        
      flushBuffer();
        
      state.isItalic = true;

      if (!state.isBold) {        
        DPRINT("add style emphasis.\n");
        
        myBookReader.pushKind(EMPHASIS);
        myBookReader.addControl(EMPHASIS, true);
      } else {
        DPRINT("add style emphasis and strong.\n");
        
        myBookReader.popKind();
        myBookReader.addControl(STRONG, false);
        
        myBookReader.pushKind(EMPHASIS);
        myBookReader.addControl(EMPHASIS, true);
        myBookReader.pushKind(STRONG);
        myBookReader.addControl(STRONG, true);
      }
      break;
    case _BOOK_TITLE:
    case _STYLE_SHEET:
    case _STYLE_INFO:
    case _TITLE_INFO:
    case _AUTHOR:
    case _ENCODING:
      state.readState = READ_NONE;
      break;
    case _STYLE_SET:
      state.style = 0;
      break;
    case _IMAGE:
      DPRINT("image start.\n");
    
      flushBuffer();

      if (!state.isPrevImage) {
        myBookReader.endParagraph();
      }

      state.isPrevImage = true;
        
      state.readState = READ_IMAGE;

      break;
		case _IMAGE_TYPE:
			break;
    case _FOOTNOTE:
    {
      flushBuffer();

  		std::string id;
			ZLStringUtil::appendNumber(id, footnoteIndex++);

      stack.push_back(state);
      state.id = id;
      state.readState = READ_TEXT;
      state.isItalic = false;
      state.isBold = false;
      state.style = -1;
      state.isPrevImage = false;
      
      myBookReader.addHyperlinkControl(FOOTNOTE, id);        
      myBookReader.addData(id);
      myBookReader.addControl(FOOTNOTE, false);
      
      myBookReader.setFootnoteTextModel(id);
      myBookReader.pushKind(REGULAR);
      myBookReader.beginParagraph();
      
      footnoteIndex++;
      break;
    }
    default:
      state.readState = READ_TEXT;
      break;
  }
}

void RtfBookReader::endElementHandler(int tag) {
  switch(tag) {
    case _BOLD:
      DPRINT("bold end.\n");

      if (state.readState != READ_TEXT) {
        DPRINT("change style not in text.\n");
        break;
      }
        
      flushBuffer();
        
      state.isBold = false;
        
      DPRINT("remove style strong.\n");
        
      myBookReader.addControl(STRONG, false);
      myBookReader.popKind();
        
      break;        
    case _ITALIC:
      DPRINT("italic end.\n");
        
      if (state.readState != READ_TEXT) {
        DPRINT("change style not in text.\n");
        break;
      }
        
      flushBuffer();
        
      state.isItalic = false;

      if (!state.isBold) {        
        DPRINT("remove style emphasis.\n");
        
        myBookReader.addControl(EMPHASIS, false);
        myBookReader.popKind();
      } else {
        DPRINT("remove style strong n emphasis, add strong.\n");
        
        myBookReader.addControl(STRONG, false);
        myBookReader.popKind();
        myBookReader.addControl(EMPHASIS, false);
        myBookReader.popKind();
        
        myBookReader.pushKind(STRONG);
        myBookReader.addControl(STRONG, true);
      }
        
      break;
    case _TITLE_INFO:
    case _AUTHOR:
    case _ENCODING:
    case _BOOK_TITLE:
    case _STYLE_INFO:
      state.readState = READ_TEXT;
      break;
    case _STYLE_SET:
      break;
    case _IMAGE:
      DPRINT("image end.\n");
      state.readState = READ_TEXT;
      break;
    case _FOOTNOTE:
      DPRINT("footnote end.\n");
        
      flushBuffer();
      myBookReader.endParagraph();
      myBookReader.popKind();
      
      state = stack.back();
      stack.pop_back();
      
      if (state.id == "") {
        myBookReader.setMainTextModel();
      } else {
        myBookReader.setFootnoteTextModel(state.id);
      }
      
      break;
    default:
      break;
  }
}
