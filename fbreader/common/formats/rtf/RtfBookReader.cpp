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

#include "RtfBookReader.h"
#include "../../bookmodel/BookModel.h"
#include "RtfImage.h"

RtfBookReader::RtfBookReader(BookModel &model, const std::string &encoding) 
    : RtfReader(encoding), BookReader(model) {
}

void RtfBookReader::addChar(const char ch) {
  if ((state.state == READ_TEXT) || (state.state == READ_IMAGE)) {
    outputBuffer += ch;
  }
}

void RtfBookReader::addCharData(const char *data, size_t len) {
  if ((state.state == READ_TEXT) || (state.state == READ_IMAGE)) {
    outputBuffer.append(data, len);
  }
}

void RtfBookReader::flushBuffer() {
  if (outputBuffer.size() > 0) {
    if (state.state == READ_TEXT) {    
      std::string newString;
    
      myConverter->convert(newString, outputBuffer.data(), outputBuffer.data() + outputBuffer.length());
      characterDataHandler(newString);
    } else if (state.state == READ_IMAGE) {
      characterDataHandler(outputBuffer);
    }
    outputBuffer.clear();
	}
}

void RtfBookReader::insertImage(const std::string &fileName, size_t startOffset, size_t size) {
  ZLImage *image = new RtfImage("image/jpeg", fileName, startOffset, size);

  std::string id;
  char tmp[256];

  sprintf(tmp, "InternalImage%i", imageIndex);

  id.append(tmp);
  addImageReference(id);   
  addImage(id, image);
  imageIndex++;
}

bool RtfBookReader::characterDataHandler(std::string &str) {
//    DPRINT("%c", str[0]);

    switch (state.state)
    {
    case READ_TEXT:
    {
        if (state.isPrevImage)
        {
        beginParagraph();
        state.isPrevImage = false;
        }
        
        addData(str);
        break;
    }
    case READ_IMAGE:
    {
        break;
    }
/*
    case READ_STYLE_INFO:
    {
        if (currentStyleInfo != NULL) {
        currentStyleInfo->name.append(str);
        }
        break;
    }
*/    
    }
    return true;
}

void RtfBookReader::startDocumentHandler() {
    DPRINT("start doc handler\n");
    
    imageIndex = 0;
    
    footnoteIndex = 1;
    stack.reserve(3);
    currentStyleInfo = 0;    
    
    state.state = READ_TEXT;
    state.isItalic = false;
    state.isBold = false;
    state.id = "";
    state.style = -1;
    state.isPrevImage = false;

    setMainTextModel();
    pushKind(REGULAR);
    beginParagraph();
}

void RtfBookReader::endDocumentHandler() {
    DPRINT("end doc handler\n");

    flushBuffer();
    endParagraph();
    popKind();

    stack.reserve(0);
}

void RtfBookReader::startElementHandler(int tag, const char **attributes) {
//    DPRINT("start handler: %i\n", tag);
    switch(tag)
    {
    case _P:
    {
        flushBuffer();
        endParagraph();
        beginParagraph();
        break;
    }
    case _BOLD:
    {
        DPRINT("bold start.\n");
        
        if (state.state != READ_TEXT)
        {
        DPRINT("change style not in text.\n");
        break;
        }
        
        flushBuffer();
        
        state.isBold = true;
        
        DPRINT("add style strong.\n");
        
        pushKind(STRONG);
        addControl(STRONG, true);

        break;        
    }
    case _ITALIC:
    {
        DPRINT("italic start.\n");
        
        if (state.state != READ_TEXT)
        {
        DPRINT("change style not in text.\n");
        break;
        }
        
        flushBuffer();
        
        state.isItalic = true;

        if (!state.isBold)
        {        
        DPRINT("add style emphasis.\n");
        
        pushKind(EMPHASIS);
        addControl(EMPHASIS, true);
        }
        else
        {
        DPRINT("add style emphasis and strong.\n");
        
        popKind();
        addControl(STRONG, false);
        
        pushKind(EMPHASIS);
        addControl(EMPHASIS, true);
        pushKind(STRONG);
        addControl(STRONG, true);
        }
        break;
    }
    case _TITLE_INFO:
    case _AUTHOR:
    case _ENCODING:
    {
        state.state = READ_NONE;
        break;
    }
    case _BOOK_TITLE:
    {
//        state = READ_CONTENT;
        state.state = READ_NONE;
        break;
    }
    case _STYLE_INFO:
    {
        state.state = READ_NONE;

/*
        flusBuffer();
        
        currentStyleInfo = new StyleInfo;
        currentStyleInfo.id = atoi(attribute[0]);
        
        styles.push_back(currentStyleInfo);
        
        state.state = READ_STYLE_INFO;
*/
    
        break;
    }
    case _STYLE_SET:
    {
        state.style = 0;
        break;
    }
    case _IMAGE:
    {
        DPRINT("image start.\n");
    
        flushBuffer();

        if(!state.isPrevImage)        
        {
        endParagraph();
        }

        state.isPrevImage = true;
        
        state.state = READ_IMAGE;

        break;
    }
    case _IMAGE_TYPE:
    {
        std::string id;
        char tmp[256];

        DPRINT("add image: %i, type: %s\n", imageIndex, attributes[0]);
        
        if ((attributes == NULL) || (attributes[0] == NULL))
        {
        break;
        }
        
        DPRINT("add image done\n");
        break;
    }
    case _FOOTNOTE:
    {
        std::string id;
        std::string id_name;
        char tmp[256];

        flushBuffer();
        sprintf(tmp, "%i", footnoteIndex);
        id.append(tmp);

        sprintf(tmp, "foonoteIndex%i", footnoteIndex);
        id_name.append(tmp);
        
        DPRINT("footnote reference: %s\n", tmp);

        
        stack.push_back(state);
        state.id = id_name;
        state.state = READ_TEXT;
        state.isItalic = false;
        state.isBold = false;
        state.style = -1;
        state.isPrevImage = false;
        
        addHyperlinkControl(FOOTNOTE, id_name);        
        addData(id);
        addControl(FOOTNOTE, false);
        
        setFootnoteTextModel(id_name);
        pushKind(REGULAR);
        beginParagraph();
        
        footnoteIndex++;
        break;
    }
    default:
        state.state = READ_TEXT;
        break;
    }
}

void RtfBookReader::endElementHandler(int tag) {
//    DPRINT("end handler: %i\n", tag);

    switch(tag)
    {
    case _BOLD:
    {
        DPRINT("bold end.\n");
        
        if (state.state != READ_TEXT)
        {
        DPRINT("change style not in text.\n");
        break;
        }
        
        flushBuffer();
        
        state.isBold = false;
        
        DPRINT("remove style strong.\n");
        
        addControl(STRONG, false);
        popKind();
        
        break;        
    }
    case _ITALIC:
    {
        DPRINT("italic end.\n");
        
        if (state.state != READ_TEXT)
        {
        DPRINT("change style not in text.\n");
        break;
        }
        
        flushBuffer();
        
        state.isItalic = false;

        if (!state.isBold)
        {        
        DPRINT("remove style emphasis.\n");
        
        addControl(EMPHASIS, false);
        popKind();
        }
        else
        {
        DPRINT("remove style strong n emphasis, add strong.\n");
        
        addControl(STRONG, false);
        popKind();
        addControl(EMPHASIS, false);
        popKind();
        
        pushKind(STRONG);
        addControl(STRONG, true);
        }
        
        break;
    }
    case _TITLE_INFO:
    case _AUTHOR:
    case _ENCODING:
    case _BOOK_TITLE:
    {
        state.state = READ_TEXT;
        break;
    }
    case _STYLE_INFO:
    {
        state.state = READ_TEXT;
        break;
    }
    case _STYLE_SET:
    {
        break;
    }
    case _IMAGE:
    {
        DPRINT("image end.\n");
        
        flushBuffer();
        
        state.state = READ_TEXT;
        
        break;
    }
    case _FOOTNOTE:
    {
        DPRINT("footnote end.\n");
        
        flushBuffer();
        endParagraph();
        popKind();
        
        state = stack.back();
        stack.pop_back();
        
        if (state.id == "")
        {
        setMainTextModel();
        }
        else
        {
        setFootnoteTextModel(state.id);
        }
        
        break;
    }
    default:
        break;
    }
}
