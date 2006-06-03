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

#include <cctype>

#include <abstract/ZLFileImage.h>

#include "../txt/PlainTextFormat.h"
#include "HtmlBookReader.h"
#include "../../bookmodel/BookModel.h"

HtmlBookReader::HtmlBookReader(const std::string &baseDirectoryPath, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlReader(encoding), myBookReader(model), myBaseDirPath(baseDirectoryPath), myFormat(format) {
}

void HtmlBookReader::addConvertedDataToBuffer(const char *text, int len, bool convert) {
  if (len > 0) {
    if (convert) {
      myConverter->convert(myConverterBuffer, text, text + len);
      myBookReader.addData(myConverterBuffer);
      myBookReader.addContentsData(myConverterBuffer);
      myConverterBuffer.erase();
    } else {
      std::string strText(text, len);
      myBookReader.addData(strText);
      myBookReader.addContentsData(strText);
    }
  }
}

bool HtmlBookReader::tagHandler(HtmlTag tag) {
  myConverter->reset();
  switch (tag.Code) {
    case _BODY:
      break;
    case _IMAGE:
      myBookReader.endParagraph();
      for (unsigned int i = 0; i < tag.Attributes.size(); i++) {
        if (tag.Attributes[i].Name == "SRC") {
          std::string fileName = tag.Attributes[i].Value;
          myBookReader.addImageReference(fileName);
          ZLImage *image = new ZLFileImage("image/auto", myBaseDirPath + fileName, 0);
          myBookReader.addImage(fileName, image);
        }
      }
      myBookReader.beginParagraph();
      break;
    // 9. text
    case _EM:
      myBookReader.addControl(EMPHASIS, tag.Start);
      break;
    case _STRONG:
      myBookReader.addControl(STRONG, tag.Start);
      break;
    case _DFN:
      //TODO: implement
      break;
    case _CODE:
      myBookReader.addControl(CODE, tag.Start);
      break;
    case _SAMP:
      //TODO: implement
      break;
    case _KBD:
      //TODO: implement
      break;
    case _VAR:
      //TODO: implement
      break;
    case _CITE:
      myBookReader.addControl(CITE, tag.Start);
      break;
    case _ABBR:
      //TODO: implement
      break;
    case _ACRONYM:
      //TODO: implement
      break;
    case _BLOCKQUOTE:
      //TODO: implement
      break;
    case _Q:
      //TODO: implement
      break;
    case _SUB:
      myBookReader.addControl(SUB, tag.Start);
      break;
    case _SUP:
      myBookReader.addControl(SUP, tag.Start);
      break;
    case _DIV:
      if (!tag.Start) {
        myBookReader.endParagraph();
        myBookReader.beginParagraph();
      }
      break;
    case _P:
    case _BR:
      myBookReader.endParagraph();
      myBookReader.beginParagraph();
      break;
    case _PRE:
      myBookReader.endParagraph();
      myIsPreformatted = tag.Start;
      mySpaceCounter = -1;
      myBreakCounter = 0;
      if (myFormat.breakType() == PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
        if (tag.Start) {
          myBookReader.pushKind(PREFORMATTED);
        } else {
          myBookReader.popKind();
        }
      }
      myBookReader.beginParagraph();
      break;
    case _INS:
      //TODO: implement
      break;
    case _DEL:
      //TODO: implement
      break;
    // 10. lists
    case _UL:
    case _OL:
      if (tag.Start) {
        myListNumStack.push((tag.Code == _UL) ? 0 : 1);
      } else if (!myListNumStack.empty()) {
        myListNumStack.pop();
      }
      break;
    case _LI:
      if (tag.Start) {
        myBookReader.endParagraph();
        myBookReader.beginParagraph();
        if (!myListNumStack.empty()) {
          //TODO: add spaces and number/bullet
          addConvertedDataToBuffer("\342\200\242 ", 4, false);
        }
      }
      break;
    case _DT:
      if (tag.Start) {
        myBookReader.endParagraph();
        myBookReader.beginParagraph();
      }
      break;
    case _DL:
    case _DD:
      break;
    //
    case _H1:
    case _H2:
    case _H3:
    case _H4:
    case _H5:
    case _H6:
      myBookReader.endParagraph();
      if (tag.Start) {
        myBookReader.insertEndOfSectionParagraph();
        myBookReader.enterTitle();
        myBookReader.beginContentsParagraph();
        switch (tag.Code) {
          case _H1:
            myBookReader.pushKind(H1);
          case _H2:
            myBookReader.pushKind(H2);
          case _H3:
            myBookReader.pushKind(H3);
          case _H4:
            myBookReader.pushKind(H4);
          case _H5:
            myBookReader.pushKind(H5);
          case _H6:
            myBookReader.pushKind(H6);
          default:
            break;
        }
      } else {
        myBookReader.popKind();
        myBookReader.endContentsParagraph();
        myBookReader.exitTitle();
      }
      myBookReader.beginParagraph();
      break;
    case _TT:
      myBookReader.addControl(CODE, tag.Start);
      break;
    case _B:
      myBookReader.addControl(BOLD, tag.Start);
      break;
    case _I:
      myBookReader.addControl(ITALIC, tag.Start);
      break;
    case _HEAD:
    case _TITLE:
    case _STYLE:
      if (tag.Start) {
        myIgnoreDataCounter++;
      } else {
        myIgnoreDataCounter--;
      }
      break;
    case _SELECT:
    case _SCRIPT:
      if (tag.Start) {
        myIgnoreDataCounter++;
      } else {
        myIgnoreDataCounter--;
      }
      break;
    case _A:
      if (tag.Start) {
        for (unsigned int i = 0; i < tag.Attributes.size(); i++) {
          if (tag.Attributes[i].Name == "NAME") {
            myBookReader.addHyperlinkLabel(tag.Attributes[i].Value);
          } else if (!myIsHyperlink && (tag.Attributes[i].Name == "HREF")) {
            const std::string &value = tag.Attributes[i].Value;
            if (!value.empty() && (value[0] == '#')) {
              myBookReader.addHyperlinkControl(HYPERLINK, value.substr(1));
              myIsHyperlink = true;
            }
          }
        }
      } else if (myIsHyperlink) {
        myBookReader.addControl(HYPERLINK, false);
        myIsHyperlink = false;
      }
      break;
    case _UNKNOWN:
      break;
  }
  return true;
}

bool HtmlBookReader::characterDataHandler(const char *text, int len, bool convert) {
  if (myIgnoreDataCounter == 0) {
    const char *start = text;
    const char *end = text + len;
    if (myIsPreformatted) {
      int breakType = myFormat.breakType();
      if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
        for (const char *ptr = text; ptr != end; ptr++) {
          if (*ptr == '\n') {
            if (start < ptr) {
              addConvertedDataToBuffer(start, ptr - start, convert);
            } else {
              static const std::string SPACE = " ";
              myBookReader.addData(SPACE);
            }
            myBookReader.endParagraph();
            myBookReader.beginParagraph();
            start = ptr + 1;
          }
        }
        addConvertedDataToBuffer(start, end - start, convert);
      } else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT) {
        for (const char *ptr = text; ptr != end; ptr++) {
          if (isspace(*ptr)) {
            if (*ptr == '\n') {
              mySpaceCounter = 0;
            } else if (mySpaceCounter >= 0) {
              mySpaceCounter++;
            }
          } else {
            if (mySpaceCounter > myFormat.ignoredIndent()) {
              if (ptr - start > mySpaceCounter) {
                addConvertedDataToBuffer(start, ptr - start - mySpaceCounter, convert);
                myBookReader.endParagraph();
                myBookReader.beginParagraph();
              }
              start = ptr;
            }
            mySpaceCounter = -1;
          }
        }
        mySpaceCounter = std::max(mySpaceCounter, 0);
        if (end - start > mySpaceCounter) {
          addConvertedDataToBuffer(start, end - start - mySpaceCounter, convert);
        }
      } else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE) {
        for (const char *ptr = start; ptr != end; ptr++) {
          if (isspace(*ptr)) {
            if (*ptr == '\n') {
              myBreakCounter++;
            }
          } else {
            if (myBreakCounter > 1) {
              addConvertedDataToBuffer(start, ptr - start, convert);
              myBookReader.endParagraph();
              myBookReader.beginParagraph();
              start = ptr;
            }
            myBreakCounter = 0;
          }
        }
        addConvertedDataToBuffer(start, end - start, convert);
      }
    } else {
      if (!myIsStarted) {
        for (; start != end; start++) {
          if (!isspace(*start)) {
            myIsStarted = true;
            break;
          }
        }
      }
      addConvertedDataToBuffer(start, end - start, convert);
    }
  }
  return true;
}

void HtmlBookReader::startDocumentHandler() {
  myBookReader.setMainTextModel();
  myBookReader.pushKind(REGULAR);
  myBookReader.beginParagraph();
  myIgnoreDataCounter = 0;
  myIsPreformatted = false;
  myIsHyperlink = false;
  myIsStarted = false;

  mySpaceCounter = -1;
  myBreakCounter = 0;
}

void HtmlBookReader::endDocumentHandler() {
  myBookReader.endParagraph();
}
