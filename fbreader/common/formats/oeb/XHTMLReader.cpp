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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLDir.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringUtil.h>

#include "XHTMLReader.h"

#include "../../bookmodel/BookReader.h"
#include "../../Files.h"

std::map<std::string,TagAction*> XHTMLReader::ourTagActions;

TagAction::~TagAction() {
}

class TagParagraphAction : public TagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class TagItemAction : public TagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class TagHyperlinkAction : public TagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class TagControlAction : public TagAction {

public:
  TagControlAction(TextKind control);

  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);

private:
  TextKind myControl;
};

class TagParagraphWithControlAction : public TagAction {

public:
  TagParagraphWithControlAction(TextKind control);

  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);

private:
  TextKind myControl;
};

static const std::string HASH = "#";

void TagAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  const char *id = reader.attributeValue(xmlattributes, "id");
  if (id != 0) {
    reader.myModelReader.addHyperlinkLabel(reader.myFileName + HASH + id);
  }
}

void TagParagraphAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.beginParagraph();
  TagAction::doAtStart(reader, xmlattributes);
}

void TagParagraphAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.endParagraph();
}

void TagItemAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  if (reader.myModelReader.paragraphIsOpen()) {
    reader.myModelReader.endParagraph();
  }
  // TODO: increase left indent
  reader.myModelReader.beginParagraph();
  TagAction::doAtStart(reader, xmlattributes);
  // TODO: replace bullet sign by number inside OL tag
  const std::string bullet = "\xE2\x80\xA2\xC0\xA0";
  reader.myModelReader.addData(bullet);
}

void TagItemAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.endParagraph();
}

TagControlAction::TagControlAction(TextKind control) : myControl(control) {
}

void TagControlAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.addControl(myControl, true);
  TagAction::doAtStart(reader, xmlattributes);
}

void TagControlAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(myControl, false);
}

void TagHyperlinkAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  std::string link;
  const char *href = reader.attributeValue(xmlattributes, "href");
  if (href != 0) {
    link = (*href == '#') ? (reader.myFileName + href) : href;
  }
  reader.myModelReader.addHyperlinkControl(HYPERLINK, link);
  TagAction::doAtStart(reader, xmlattributes);
}

void TagHyperlinkAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(HYPERLINK, false);
}

TagParagraphWithControlAction::TagParagraphWithControlAction(TextKind control) : myControl(control) {
}

void TagParagraphWithControlAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  if (myControl == TITLE) {
    reader.myModelReader.insertEndOfSectionParagraph();
  }
  reader.myModelReader.beginParagraph();
  reader.myModelReader.addControl(myControl, true);
  TagAction::doAtStart(reader, xmlattributes);
}

void TagParagraphWithControlAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(myControl, false);
  reader.myModelReader.endParagraph();
}

void XHTMLReader::fillTagTable() {
  if (ourTagActions.empty()) {
    //ourTagActions["html"] = new TagAction();
    //ourTagActions["body"] = new TagAction();
    //ourTagActions["title"] = new TagAction();
    //ourTagActions["meta"] = new TagAction();
    //ourTagActions["script"] = new TagAction();

    //ourTagActions["font"] = new TagAction();
    //ourTagActions["style"] = new TagAction();

    ourTagActions["p"] = new TagParagraphAction();
    ourTagActions["h1"] = new TagParagraphWithControlAction(TITLE);
    ourTagActions["h2"] = new TagParagraphWithControlAction(TITLE);
    ourTagActions["h3"] = new TagParagraphWithControlAction(SUBTITLE);
    ourTagActions["h4"] = new TagParagraphWithControlAction(SUBTITLE);
    ourTagActions["h5"] = new TagParagraphWithControlAction(STRONG);
    ourTagActions["h6"] = new TagParagraphWithControlAction(STRONG);

    //ourTagActions["ol"] = new TagAction();
    //ourTagActions["ul"] = new TagAction();
    //ourTagActions["dl"] = new TagAction();
    ourTagActions["li"] = new TagItemAction();

    ourTagActions["strong"] = new TagControlAction(STRONG);
    ourTagActions["b"] = new TagControlAction(BOLD);
    ourTagActions["em"] = new TagControlAction(EMPHASIS);
    ourTagActions["i"] = new TagControlAction(ITALIC);
    ourTagActions["code"] = new TagControlAction(CODE);
    ourTagActions["tt"] = new TagControlAction(CODE);
    ourTagActions["kbd"] = new TagControlAction(CODE);
    ourTagActions["var"] = new TagControlAction(CODE);
    ourTagActions["samp"] = new TagControlAction(CODE);
    ourTagActions["cite"] = new TagControlAction(CITE);
    ourTagActions["sub"] = new TagControlAction(SUB);
    ourTagActions["sup"] = new TagControlAction(SUP);
    ourTagActions["dd"] = new TagControlAction(DEFINITION_DESCRIPTION);
    ourTagActions["dfn"] = new TagControlAction(DEFINITION);
    ourTagActions["strike"] = new TagControlAction(STRIKETHROUGH);

    ourTagActions["a"] = new TagHyperlinkAction();
    //ourTagActions["img"] = new TagAction();
    //ourTagActions["object"] = new TagAction();

    //ourTagActions["area"] = new TagAction();
    //ourTagActions["map"] = new TagAction();

    //ourTagActions["base"] = new TagAction();
    //ourTagActions["blockquote"] = new TagAction();
    //ourTagActions["br"] = new TagAction();
    //ourTagActions["center"] = new TagAction();
    //ourTagActions["div"] = new TagAction();
    //ourTagActions["dt"] = new TagAction();
    //ourTagActions["head"] = new TagAction();
    //ourTagActions["hr"] = new TagAction();
    //ourTagActions["link"] = new TagAction();
    //ourTagActions["param"] = new TagAction();
    //ourTagActions["q"] = new TagAction();
    //ourTagActions["s"] = new TagAction();

    //ourTagActions["pre"] = new TagAction();
    //ourTagActions["big"] = new TagAction();
    //ourTagActions["small"] = new TagAction();
    //ourTagActions["u"] = new TagAction();

    //ourTagActions["table"] = new TagAction();
    //ourTagActions["td"] = new TagAction();
    //ourTagActions["th"] = new TagAction();
    //ourTagActions["tr"] = new TagAction();
    //ourTagActions["caption"] = new TagAction();
    //ourTagActions["span"] = new TagAction();
  }
}

XHTMLReader::XHTMLReader(BookReader &modelReader) : myModelReader(modelReader) {
}

bool XHTMLReader::readFile(const std::string &pathPrefix, const std::string &name) {
  fillTagTable();

  myFileName = name;
  myModelReader.addHyperlinkLabel(myFileName);

  shared_ptr<ZLInputStream> stream = ZLFile(pathPrefix + name).inputStream();

  return readDocument(stream);
}


void XHTMLReader::startElementHandler(const char *tag, const char **attributes) {
  // todo: lowercase
  TagAction *action = ourTagActions[tag];
  if (action != 0) {
    action->doAtStart(*this, attributes);
  }
}

void XHTMLReader::endElementHandler(const char *tag) {
  // todo: lowercase
  TagAction *action = ourTagActions[tag];
  if (action != 0) {
    action->doAtEnd(*this);
  }
}

void XHTMLReader::characterDataHandler(const char *text, int len) {
  if (myModelReader.paragraphIsOpen()) {
    myModelReader.addData(std::string(text,len));
  }
}

static std::vector<std::string> EXTERNAL_DTDs;

const std::vector<std::string> &XHTMLReader::externalDTDs() const {
  if (EXTERNAL_DTDs.empty()) {
    std::string directoryName =
      Files::PathPrefix + Files::PathDelimiter +
      "formats" + Files::PathDelimiter + "xhtml";
    shared_ptr<ZLDir> dtdPath = ZLFile(directoryName).directory();
    if (!dtdPath.isNull()) {
      std::vector<std::string> files;
      dtdPath->collectFiles(files, false);
      for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); it++) {
        if (ZLStringUtil::stringEndsWith(*it, ".ent")) {
          EXTERNAL_DTDs.push_back(dtdPath->itemName(*it));
          std::cerr << EXTERNAL_DTDs.back() << "\n";
        }
      }
    }
  }

  return EXTERNAL_DTDs;
}
