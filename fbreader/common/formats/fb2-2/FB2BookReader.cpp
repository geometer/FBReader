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

#include "FB2BookReader.h"
#include "Base64EncodedImage.h"
#include "../../bookmodel/BookModel.h"
#include "../../model/Paragraph.h"

std::map<std::string,FB2TagAction*> FB2BookReader::ourTagActions;

class FB2TagControlAction : public FB2TagAction {

public:
  FB2TagControlAction(TextKind control);

  void doAtStart(FB2BookReader &reader, const char **xmlattributes);
  void doAtEnd(FB2BookReader &reader);

private:
  TextKind myControl;
};

class FB2TagParagraphWithControlAction : public FB2TagAction {

public:
  FB2TagParagraphWithControlAction(TextKind control);

  void doAtStart(FB2BookReader &reader, const char **xmlattributes);
  void doAtEnd(FB2BookReader &reader);

private:
  TextKind myControl;
};

void FB2TagAction::doAtStart(FB2BookReader &reader, const char **xmlattributes) {
  const char *id = reader.attributeValue(xmlattributes, "id");
  if (id != 0) {
    if (reader.myBodyCounter > 1) {
      reader.myModelReader.setFootnoteTextModel(id);
    } else {
      reader.myModelReader.addHyperlinkLabel(id);
    }
  }
}

FB2TagControlAction::FB2TagControlAction(TextKind control) : myControl(control) {
}

void FB2TagControlAction::doAtStart(FB2BookReader &reader, const char **xmlattributes) {
  reader.myModelReader.addControl(myControl, true);
  FB2TagAction::doAtStart(reader, xmlattributes);
}

void FB2TagControlAction::doAtEnd(FB2BookReader &reader) {
  reader.myModelReader.addControl(myControl, false);
}

FB2TagParagraphWithControlAction::FB2TagParagraphWithControlAction(TextKind control) : myControl(control) {
}

void FB2TagParagraphWithControlAction::doAtStart(FB2BookReader &reader, const char **xmlattributes) {
  reader.myModelReader.beginParagraph();
  reader.myModelReader.addControl(myControl, true);
  FB2TagAction::doAtStart(reader, xmlattributes);
}

void FB2TagParagraphWithControlAction::doAtEnd(FB2BookReader &reader) {
  reader.myModelReader.addControl(myControl, false);
  reader.myModelReader.endParagraph();
}

static const std::string _P = "p";
static const std::string _CITE = "cite";
static const std::string _SECTION = "section";
static const std::string _TITLE = "title";
static const std::string _POEM = "poem";
static const std::string _STANZA = "stanza";
static const std::string _EPIGRAPH = "epigraph";
static const std::string _ANNOTATION = "annotation";
static const std::string _A = "a";
static const std::string _IMAGE = "image";
static const std::string _BINARY = "binary";
static const std::string _BODY = "body";
static const std::string _EMPTY_LINE = "empty-line";

FB2BookReader::FB2BookReader(BookModel &model) : myModelReader(model) {
  myInsidePoem = false;
  mySectionDepth = 0;
  myBodyCounter = 0;
  myCurrentImage = 0;
  myProcessingImage = false;
  mySectionStarted = false;
  myInsideTitle1 = false;
}

void FB2BookReader::characterDataHandler(const char *text, int len) {
  if ((len > 0) && (myProcessingImage || myModelReader.paragraphIsOpen())) {
    std::string str(text, len);
    if (myProcessingImage) {
      myImageBuffer.push_back(str);
    } else {
      myModelReader.addData(str);
      myModelReader.addContentsData(str);
    }
  }
}

static const char *reference(const char **xmlattributes) FORMATS_SECTION;
static const char *reference(const char **xmlattributes) {
  while (*xmlattributes != 0) {
    int len = strlen(*xmlattributes);
    bool useNext = (len >= 5) && (strcmp((*xmlattributes) + len - 5, ":href") == 0);
    xmlattributes++;
    if (*xmlattributes == 0) {
      return 0;
    }
    if (useNext) {
      break;
    }
    xmlattributes++;
  }
  const char *ref = *xmlattributes;
  return ((ref != 0) && (*ref == '#')) ? ref + 1 : 0;
}
  
void FB2BookReader::startElementHandler(const char *tag, const char **xmlattributes) {
  std::map<std::string,FB2TagAction*>::const_iterator it = ourTagActions.find(tag);
  if (it != ourTagActions.end()) {
    it->second->doAtStart(*this, xmlattributes);
    return;
  }

  const char *id = attributeValue(xmlattributes, "id");
  if (id != 0) {
    if (myBodyCounter > 1) {
      myModelReader.setFootnoteTextModel(id);
    } else {
      myModelReader.addHyperlinkLabel(id);
    }
  }

  if (_P == tag) {
    if (mySectionStarted) {
      myModelReader.beginContentsParagraph();
      if (!myInsideTitle1) {
        myModelReader.endContentsParagraph();
      }
      mySectionStarted = false;
    } else if (myInsideTitle1) {
      static const std::string SPACE = " ";
      myModelReader.addContentsData(SPACE);
    }
    myModelReader.beginParagraph();
  } else if (_CITE == tag) {
    myModelReader.pushKind(CITE);
  } else if (_SECTION == tag) {
    myModelReader.insertEndOfSectionParagraph();
    mySectionDepth++;
    mySectionStarted = true;
  } else if (_TITLE == tag) {
    if (myInsidePoem) {
      myModelReader.pushKind(POEM_TITLE);
    } else if (mySectionDepth == 0) {
      myModelReader.insertEndOfSectionParagraph();
      myModelReader.pushKind(TITLE);
    } else {
      myModelReader.pushKind(SECTION_TITLE);
      myModelReader.enterTitle();
      myInsideTitle1 = true;
    }
  } else if (_POEM == tag) {
    myInsidePoem = true;
  } else if (_STANZA == tag) {
    myModelReader.pushKind(STANZA);
    myModelReader.beginParagraph(Paragraph::BEFORE_SKIP_PARAGRAPH);
    myModelReader.endParagraph();
  } else if (_EPIGRAPH == tag) {
    myModelReader.pushKind(EPIGRAPH);
  } else if (_ANNOTATION == tag) {
    if (myBodyCounter == 0) {
      myModelReader.setMainTextModel();
    }
    myModelReader.pushKind(ANNOTATION);
  } else if (_A == tag) {
    const char *ref = reference(xmlattributes);
    if (ref != 0) {
      myModelReader.addHyperlinkControl(FOOTNOTE, ref);
    } else {
      myModelReader.addControl(FOOTNOTE, true);
    }
  } else if (_IMAGE == tag) {
    const char *ref = reference(xmlattributes);
    if (ref != 0) {
      myModelReader.addImageReference(ref);
    }
  } else if (_BINARY == tag) {
    const char *contentType = attributeValue(xmlattributes, "content-type");
    const char *id = attributeValue(xmlattributes, "id");
    if ((contentType != 0) && (id != 0)) {
      myCurrentImage = new Base64EncodedImage(contentType);
      myModelReader.addImage(id, myCurrentImage);
      myProcessingImage = true;
    }
  } else if (_EMPTY_LINE == tag) {
    myModelReader.beginParagraph(Paragraph::EMPTY_LINE_PARAGRAPH);
    myModelReader.endParagraph();
  } else if (_BODY == tag) {
    myBodyCounter++;
    if (myBodyCounter == 1) {
      myModelReader.setMainTextModel();
    }
    myModelReader.pushKind(REGULAR);
  }
}

void FB2BookReader::endElementHandler(const char *tag) {
  std::map<std::string,FB2TagAction*>::const_iterator it = ourTagActions.find(tag);
  if (it != ourTagActions.end()) {
    it->second->doAtEnd(*this);
    return;
  }

  if (_P == tag) {
    myModelReader.endParagraph();
  } else if (_CITE == tag) {
    myModelReader.popKind();
  } else if (_SECTION == tag) {
    if (myBodyCounter > 1) {
      myModelReader.unsetTextModel();
    }
    mySectionDepth--;
    mySectionStarted = false;
  } else if (_TITLE == tag) {
    myModelReader.exitTitle();
    myModelReader.popKind();
    myModelReader.endContentsParagraph();
    myInsideTitle1 = false;
  } else if (_POEM == tag) {
    myInsidePoem = false;
  } else if (_STANZA == tag) {
    myModelReader.beginParagraph(Paragraph::AFTER_SKIP_PARAGRAPH);
    myModelReader.endParagraph();
    myModelReader.popKind();
  } else if (_EPIGRAPH == tag) {
    myModelReader.popKind();
  } else if (_ANNOTATION == tag) {
    myModelReader.popKind();
    if (myBodyCounter == 0) {
      myModelReader.insertEndOfSectionParagraph();
      myModelReader.unsetTextModel();
    }
  } else if (_A == tag) {
    myModelReader.addControl(FOOTNOTE, false);
  } else if (_BINARY == tag) {
    if (!myImageBuffer.empty() && (myCurrentImage != 0)) {
      myCurrentImage->addData(myImageBuffer);
      myImageBuffer.clear();
      myCurrentImage = 0;
    }
    myProcessingImage = false;
  } else if (_BODY == tag) {
    myModelReader.popKind();
    myModelReader.unsetTextModel();
  }
}

bool FB2BookReader::readBook(shared_ptr<ZLInputStream> stream) {
  fillTagTable();
  return readDocument(stream);
}

void FB2BookReader::addAction(const std::string &tag, FB2TagAction *action) {
  ourTagActions.insert(std::pair<std::string,FB2TagAction*>(tag, action));
}

void FB2BookReader::fillTagTable() {
  if (ourTagActions.empty()) {
    addAction("sub",  new FB2TagControlAction(SUB));
    addAction("sup",  new FB2TagControlAction(SUP));
    addAction("code",  new FB2TagControlAction(CODE));
    addAction("strikethrough",  new FB2TagControlAction(STRIKETHROUGH));
    addAction("strong",  new FB2TagControlAction(STRONG));
    addAction("emphasis",  new FB2TagControlAction(EMPHASIS));
    addAction("v",  new FB2TagParagraphWithControlAction(VERSE));
    addAction("subtitle",  new FB2TagParagraphWithControlAction(SUBTITLE));
    addAction("text-author",  new FB2TagParagraphWithControlAction(AUTHOR));
    addAction("date",  new FB2TagParagraphWithControlAction(DATE));
  }
}
