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
  virtual void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) = 0;

protected:
  HtmlBookReader &myReader;
};

class HtmlControlTagAction : public HtmlTagAction {

public:
  HtmlControlTagAction(HtmlBookReader &reader, TextKind kind);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
  TextKind myKind;
};

class HtmlHeaderTagAction : public HtmlTagAction {

public:
  HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
  TextKind myKind;
};

class HtmlIgnoreTagAction : public HtmlTagAction {

public:
  HtmlIgnoreTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlHrefTagAction : public HtmlTagAction {

public:
  HtmlHrefTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlImageTagAction : public HtmlTagAction {

public:
  HtmlImageTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlBreakTagAction : public HtmlTagAction {

public:
  enum BreakType {
    BREAK_AT_START = 1,
    BREAK_AT_END = 2,
    BREAK_AT_START_AND_AT_END = BREAK_AT_START | BREAK_AT_END
  };
  HtmlBreakTagAction(HtmlBookReader &reader, BreakType breakType);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);

private:
  BreakType myBreakType;
};

class HtmlPreTagAction : public HtmlTagAction {

public:
  HtmlPreTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlListTagAction : public HtmlTagAction {

public:
  HtmlListTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

class HtmlListItemTagAction : public HtmlTagAction {

public:
  HtmlListItemTagAction(HtmlBookReader &reader);
  void run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes);
};

HtmlTagAction::HtmlTagAction(HtmlBookReader &reader) : myReader(reader) {
}

HtmlTagAction::~HtmlTagAction() {
}

HtmlControlTagAction::HtmlControlTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlControlTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if (start) {
    myReader.myBookReader.pushKind(myKind);
    myReader.myKindList.push_back(myKind);
    myReader.myBookReader.addControl(myKind, true);
  } else {
    std::vector<TextKind> &list = myReader.myKindList;
    if (!list.empty()) {
      int index;
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
        for (unsigned int j = index + 1; j < list.size(); j++) {
          myReader.myBookReader.addControl(list[j], true);
          myReader.myBookReader.pushKind(list[j]);
        }
        list.erase(list.begin() + index);
      }
    }
  }
}

HtmlHeaderTagAction::HtmlHeaderTagAction(HtmlBookReader &reader, TextKind kind) : HtmlTagAction(reader), myKind(kind) {
}

void HtmlHeaderTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
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

void HtmlIgnoreTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if (start) {
    myReader.myIgnoreDataCounter++;
  } else {
    myReader.myIgnoreDataCounter--;
  }
}

HtmlHrefTagAction::HtmlHrefTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlHrefTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
  if (start) {
    for (unsigned int i = 0; i < attributes.size(); i++) {
      if (attributes[i].Name == "NAME") {
        myReader.myBookReader.addHyperlinkLabel(attributes[i].Value);
      } else if (!myReader.myIsHyperlink && (attributes[i].Name == "HREF")) {
        const std::string &value = attributes[i].Value;
        if (!value.empty() && (value[0] == '#')) {
          myReader.myBookReader.addHyperlinkControl(HYPERLINK, value.substr(1));
          myReader.myIsHyperlink = true;
        }
      }
    }
  } else if (myReader.myIsHyperlink) {
    myReader.myBookReader.addControl(HYPERLINK, false);
    myReader.myIsHyperlink = false;
  }
}

HtmlImageTagAction::HtmlImageTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlImageTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute> &attributes) {
  if (start) {
    myReader.myBookReader.endParagraph();
    for (unsigned int i = 0; i < attributes.size(); i++) {
      if (attributes[i].Name == "SRC") {
        std::string fileName = attributes[i].Value;
        myReader.myBookReader.addImageReference(fileName);
        myReader.myBookReader.addImage(fileName,
          new ZLFileImage("image/auto", myReader.myBaseDirPath + fileName, 0)
        );
        break;
      }
    }
    myReader.myBookReader.beginParagraph();
  }
}

HtmlBreakTagAction::HtmlBreakTagAction(HtmlBookReader &reader, BreakType breakType) : HtmlTagAction(reader), myBreakType(breakType) {
}

void HtmlBreakTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if ((start && (myBreakType & BREAK_AT_START)) ||
      (!start && (myBreakType & BREAK_AT_END))) {
    myReader.myBookReader.endParagraph();
    myReader.myBookReader.beginParagraph();
  }
}

HtmlPreTagAction::HtmlPreTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlPreTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  myReader.myBookReader.endParagraph();
  myReader.myIsPreformatted = start;
  myReader.mySpaceCounter = -1;
  myReader.myBreakCounter = 0;
  if (myReader.myFormat.breakType() == PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
    if (start) {
      myReader.myBookReader.pushKind(PREFORMATTED);
    } else {
      myReader.myBookReader.popKind();
    }
  }
  myReader.myBookReader.beginParagraph();
}

HtmlListTagAction::HtmlListTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlListTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if (start) {
    myReader.myListNumStack.push(0/*(tag.Code == _UL) ? 0 : 1*/);
  } else if (!myReader.myListNumStack.empty()) {
    myReader.myListNumStack.pop();
  }
}

HtmlListItemTagAction::HtmlListItemTagAction(HtmlBookReader &reader) : HtmlTagAction(reader) {
}

void HtmlListItemTagAction::run(bool start, const std::vector<HtmlReader::HtmlAttribute>&) {
  if (start) {
    myReader.myBookReader.endParagraph();
    myReader.myBookReader.beginParagraph();
    if (!myReader.myListNumStack.empty()) {
      //TODO: add spaces and number/bullet
      myReader.addConvertedDataToBuffer("\342\200\242 ", 4, false);
    }
  }
}

HtmlBookReader::HtmlBookReader(const std::string &baseDirectoryPath, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlReader(encoding), myBookReader(model), myBaseDirPath(baseDirectoryPath), myFormat(format) {
  myActionMap["EM"] = new HtmlControlTagAction(*this, EMPHASIS);
  myActionMap["STRONG"] = new HtmlControlTagAction(*this, STRONG);
  myActionMap["B"] = new HtmlControlTagAction(*this, BOLD);
  myActionMap["I"] = new HtmlControlTagAction(*this, ITALIC);
  myActionMap["TT"] = new HtmlControlTagAction(*this, CODE);
  myActionMap["CODE"] = new HtmlControlTagAction(*this, CODE);
  myActionMap["CITE"] = new HtmlControlTagAction(*this, CITE);
  myActionMap["SUB"] = new HtmlControlTagAction(*this, SUB);
  myActionMap["SUP"] = new HtmlControlTagAction(*this, SUP);
  myActionMap["H1"] = new HtmlHeaderTagAction(*this, H1);
  myActionMap["H2"] = new HtmlHeaderTagAction(*this, H2);
  myActionMap["H3"] = new HtmlHeaderTagAction(*this, H3);
  myActionMap["H4"] = new HtmlHeaderTagAction(*this, H4);
  myActionMap["H5"] = new HtmlHeaderTagAction(*this, H5);
  myActionMap["H6"] = new HtmlHeaderTagAction(*this, H6);
  myActionMap["HEAD"] = new HtmlIgnoreTagAction(*this);
  myActionMap["TITLE"] = new HtmlIgnoreTagAction(*this);
  myActionMap["STYLE"] = new HtmlIgnoreTagAction(*this);
  myActionMap["SELECT"] = new HtmlIgnoreTagAction(*this);
  myActionMap["SCRIPT"] = new HtmlIgnoreTagAction(*this);
  myActionMap["A"] = new HtmlHrefTagAction(*this);
  myActionMap["DIV"] = new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_END);
  myActionMap["DT"] = new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START);
  myActionMap["P"] = new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START_AND_AT_END);
  myActionMap["BR"] = new HtmlBreakTagAction(*this, HtmlBreakTagAction::BREAK_AT_START_AND_AT_END);
  myActionMap["IMG"] = new HtmlImageTagAction(*this);
  myActionMap["UL"] = new HtmlListTagAction(*this);
  myActionMap["MENU"] = new HtmlListTagAction(*this);
  myActionMap["DIR"] = new HtmlListTagAction(*this);
  myActionMap["OL"] = new HtmlListTagAction(*this);
  myActionMap["LI"] = new HtmlListItemTagAction(*this);
  myActionMap["PRE"] = new HtmlPreTagAction(*this);
  // myActionMap["DD"] =
  // myActionMap["DL"] =
  // myActionMap["DFN"] =
  // myActionMap["SAMP"] =
  // myActionMap["KBD"] =
  // myActionMap["VAR"] =
  // myActionMap["ABBR"] =
  // myActionMap["ACRONYM"] =
  // myActionMap["BLOCKQUOTE"] =
  // myActionMap["Q"] =
  // myActionMap["INS"] =
  // myActionMap["DEL"] =
  // myActionMap["BODY"] =
}

HtmlBookReader::~HtmlBookReader() {
  for (std::map<std::string,HtmlTagAction*>::const_iterator it = myActionMap.begin(); it != myActionMap.end(); it++) {
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

bool HtmlBookReader::tagHandler(const HtmlTag &tag) {
  myConverter->reset();

  HtmlTagAction *action = myActionMap[tag.Name];
  if (action != 0) {
    action->run(tag.Start, tag.Attributes);
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
