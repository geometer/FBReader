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

class HtmlTagAction {

protected:
  HtmlTagAction(HtmlBookReader &reader);

public:
  virtual ~HtmlTagAction();
  virtual void run(bool start) = 0;

protected:
  HtmlBookReader &myReader;
};

class HtmlControlTagAction : public HtmlTagAction {

public:
  HtmlControlTagAction(HtmlBookReader &reader, TextKind kind);
  void run(bool start);

private:
  TextKind myKind;
};

class HtmlHeaderTagAction : public HtmlTagAction {

public:
  HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind);
  void run(bool start);

private:
  TextKind myKind;
};

class HtmlIgnoreTagAction : public HtmlTagAction {

public:
  HtmlIgnoreTagAction(HtmlBookReader &reader);
  void run(bool start);
};

HtmlTagAction::HtmlTagAction(HtmlBookReader &reader) : myReader(reader) {
}

HtmlTagAction::~HtmlTagAction() {
}

HtmlControlTagAction::HtmlControlTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlControlTagAction::run(bool start) {
  if (start) {
    myReader.myBookReader.pushKind(myKind);
    myReader.myKindList.push_back(myKind);
    myReader.myBookReader.addControl(myKind, true);
  } else {
    int index;
    std::vector<TextKind> &list = myReader.myKindList;
    for (index = list.size() - 1; index >= 0; index--) {
      if (list[index] == myKind) {
	break;
      }
    }
    if (index >= 0) {
      for (int i = list.size() - 1; i >= index; i--) {
        myReader.myBookReader.addControl(list[i], false);
        myReader.myBookReader.popKind();
      }
      for (int i = index + 1; i < list.size(); i++) {
        myReader.myBookReader.addControl(list[i], true);
        myReader.myBookReader.pushKind(list[i]);
      }
      list.erase(list.begin() + index);
    }
  }
}
  
HtmlHeaderTagAction::HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlHeaderTagAction::run(bool start) {
  myReader.myBookReader.endParagraph();
  if (start) {
    myReader.myBookReader.insertEndOfSectionParagraph();
    myReader.myBookReader.enterTitle();
    myReader.myBookReader.beginContentsParagraph();
    myReader.myBookReader.pushKind(myKind);
  } else {
    myReader.myBookReader.popKind();
    myReader.myBookReader.endContentsParagraph();
    myReader.myBookReader.exitTitle();
  }
  myReader.myBookReader.beginParagraph();
}
  
HtmlIgnoreTagAction::HtmlIgnoreTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlIgnoreTagAction::run(bool start) {
  if (start) {
    myReader.myIgnoreDataCounter++;
  } else {
    myReader.myIgnoreDataCounter--;
  }
}
  
HtmlBookReader::HtmlBookReader(const std::string &baseDirectoryPath, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlReader(encoding), myBookReader(model), myBaseDirPath(baseDirectoryPath), myFormat(format) {
  myActionMap[_EM] = new HtmlControlTagAction(*this, EMPHASIS);
  myActionMap[_STRONG] = new HtmlControlTagAction(*this, STRONG);
  myActionMap[_B] = new HtmlControlTagAction(*this, BOLD);
  myActionMap[_I] = new HtmlControlTagAction(*this, ITALIC);
  myActionMap[_TT] = new HtmlControlTagAction(*this, CODE);
  myActionMap[_CODE] = new HtmlControlTagAction(*this, CODE);
  myActionMap[_CITE] = new HtmlControlTagAction(*this, CITE);
  myActionMap[_SUB] = new HtmlControlTagAction(*this, SUB);
  myActionMap[_SUP] = new HtmlControlTagAction(*this, SUP);
  myActionMap[_H1] = new HtmlHeaderTagAction(*this, H1);
  myActionMap[_H2] = new HtmlHeaderTagAction(*this, H2);
  myActionMap[_H3] = new HtmlHeaderTagAction(*this, H3);
  myActionMap[_H4] = new HtmlHeaderTagAction(*this, H4);
  myActionMap[_H5] = new HtmlHeaderTagAction(*this, H5);
  myActionMap[_H6] = new HtmlHeaderTagAction(*this, H6);
  myActionMap[_HEAD] = new HtmlIgnoreTagAction(*this);
  myActionMap[_TITLE] = new HtmlIgnoreTagAction(*this);
  myActionMap[_STYLE] = new HtmlIgnoreTagAction(*this);
  myActionMap[_SELECT] = new HtmlIgnoreTagAction(*this);
  myActionMap[_SCRIPT] = new HtmlIgnoreTagAction(*this);
  // myActionMap[_DD] =
  // myActionMap[_DL] =
  // myActionMap[_DFN] =
  // myActionMap[_SAMP] =
  // myActionMap[_KBD] =
  // myActionMap[_VAR] =
  // myActionMap[_ABBR] =
  // myActionMap[_ACRONYM] =
  // myActionMap[_BLOCKQUOTE] =
  // myActionMap[_Q] =
  // myActionMap[_INS] =
  // myActionMap[_DEL] =
  // myActionMap[_BODY] =
}

HtmlBookReader::~HtmlBookReader() {
  for (std::map<TagCode,HtmlTagAction*>::const_iterator it = myActionMap.begin(); it != myActionMap.end(); it++) {
    delete it->second;
  }
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
    default:
      {
        HtmlTagAction *action = myActionMap[tag.Code];
	if (action != 0) {
          action->run(tag.Start);
	}
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
