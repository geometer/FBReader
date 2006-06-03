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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLDir.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFileImage.h>

#include "XHTMLReader.h"

#include "../../bookmodel/BookReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../Files.h"

std::map<std::string,XHTMLTagAction*> XHTMLReader::ourTagActions;

XHTMLTagAction::~XHTMLTagAction() {
}

class XHTMLTagParagraphAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagRestartParagraphAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagImageAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagItemAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagHyperlinkAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagControlAction : public XHTMLTagAction {

public:
  XHTMLTagControlAction(TextKind control);

  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);

private:
  TextKind myControl;
};

class XHTMLTagParagraphWithControlAction : public XHTMLTagAction {

public:
  XHTMLTagParagraphWithControlAction(TextKind control);

  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);

private:
  TextKind myControl;
};

class XHTMLTagPreAction : public XHTMLTagAction {

public:
  void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  void doAtEnd(XHTMLReader &reader);
};

static const std::string HASH = "#";

void XHTMLTagAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  const char *id = reader.attributeValue(xmlattributes, "id");
  if (id != 0) {
    reader.myModelReader.addHyperlinkLabel(reader.myFileName + HASH + id);
  }
}

void XHTMLTagParagraphAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.beginParagraph();
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagParagraphAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.endParagraph();
}

void XHTMLTagRestartParagraphAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.endParagraph();
  reader.myModelReader.beginParagraph();
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagRestartParagraphAction::doAtEnd(XHTMLReader&) {
}

void XHTMLTagItemAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.endParagraph();
  // TODO: increase left indent
  reader.myModelReader.beginParagraph();
  XHTMLTagAction::doAtStart(reader, xmlattributes);
  // TODO: replace bullet sign by number inside OL tag
  const std::string bullet = "\xE2\x80\xA2\xC0\xA0";
  reader.myModelReader.addData(bullet);
}

void XHTMLTagItemAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.endParagraph();
}

void XHTMLTagImageAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  const char *fileName = reader.attributeValue(xmlattributes, "data");
  if (fileName != 0) {
		bool flag = reader.myModelReader.paragraphIsOpen();
		if (flag) {
			reader.myModelReader.endParagraph();
		}
		if ((strlen(fileName) > 2) && strncmp(fileName, "./", 2) == 0) {
			fileName +=2;
		}
    reader.myModelReader.addImageReference(fileName);
    ZLImage *image = new ZLFileImage("image/auto", reader.myPathPrefix + fileName, 0);
    reader.myModelReader.addImage(fileName, image);
		if (flag) {
			reader.myModelReader.beginParagraph();
		}
  }
}

void XHTMLTagImageAction::doAtEnd(XHTMLReader&) {
}

XHTMLTagControlAction::XHTMLTagControlAction(TextKind control) : myControl(control) {
}

void XHTMLTagControlAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myModelReader.addControl(myControl, true);
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagControlAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(myControl, false);
}

void XHTMLTagHyperlinkAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  std::string link;
  const char *href = reader.attributeValue(xmlattributes, "href");
  if (href != 0) {
    link = (*href == '#') ? (reader.myFileName + href) : href;
  }
  reader.myModelReader.addHyperlinkControl(HYPERLINK, link);
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagHyperlinkAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(HYPERLINK, false);
}

XHTMLTagParagraphWithControlAction::XHTMLTagParagraphWithControlAction(TextKind control) : myControl(control) {
}

void XHTMLTagParagraphWithControlAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  if ((myControl == TITLE) && (reader.myModelReader.model().bookTextModel().paragraphsNumber() > 1)) {
    reader.myModelReader.insertEndOfSectionParagraph();
  }
  reader.myModelReader.beginParagraph();
  reader.myModelReader.addControl(myControl, true);
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagParagraphWithControlAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(myControl, false);
  reader.myModelReader.endParagraph();
}

void XHTMLTagPreAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
  reader.myPreformatted = true;
  reader.myModelReader.beginParagraph();
  reader.myModelReader.addControl(CODE, true);
  XHTMLTagAction::doAtStart(reader, xmlattributes);
}

void XHTMLTagPreAction::doAtEnd(XHTMLReader &reader) {
  reader.myModelReader.addControl(CODE, false);
  reader.myModelReader.endParagraph();
  reader.myPreformatted = false;
}

void XHTMLReader::fillTagTable() {
  if (ourTagActions.empty()) {
    //ourTagActions["html"] = new XHTMLTagAction();
    ourTagActions["body"] = new XHTMLTagParagraphAction();
    //ourTagActions["title"] = new XHTMLTagAction();
    //ourTagActions["meta"] = new XHTMLTagAction();
    //ourTagActions["script"] = new XHTMLTagAction();

    //ourTagActions["font"] = new XHTMLTagAction();
    //ourTagActions["style"] = new XHTMLTagAction();

    ourTagActions["p"] = new XHTMLTagParagraphAction();
    ourTagActions["h1"] = new XHTMLTagParagraphWithControlAction(H1);
    ourTagActions["h2"] = new XHTMLTagParagraphWithControlAction(H2);
    ourTagActions["h3"] = new XHTMLTagParagraphWithControlAction(H3);
    ourTagActions["h4"] = new XHTMLTagParagraphWithControlAction(H4);
    ourTagActions["h5"] = new XHTMLTagParagraphWithControlAction(H5);
    ourTagActions["h6"] = new XHTMLTagParagraphWithControlAction(H6);

    //ourTagActions["ol"] = new XHTMLTagAction();
    //ourTagActions["ul"] = new XHTMLTagAction();
    //ourTagActions["dl"] = new XHTMLTagAction();
    ourTagActions["li"] = new XHTMLTagItemAction();

    ourTagActions["strong"] = new XHTMLTagControlAction(STRONG);
    ourTagActions["b"] = new XHTMLTagControlAction(BOLD);
    ourTagActions["em"] = new XHTMLTagControlAction(EMPHASIS);
    ourTagActions["i"] = new XHTMLTagControlAction(ITALIC);
    ourTagActions["code"] = new XHTMLTagControlAction(CODE);
    ourTagActions["tt"] = new XHTMLTagControlAction(CODE);
    ourTagActions["kbd"] = new XHTMLTagControlAction(CODE);
    ourTagActions["var"] = new XHTMLTagControlAction(CODE);
    ourTagActions["samp"] = new XHTMLTagControlAction(CODE);
    ourTagActions["cite"] = new XHTMLTagControlAction(CITE);
    ourTagActions["sub"] = new XHTMLTagControlAction(SUB);
    ourTagActions["sup"] = new XHTMLTagControlAction(SUP);
    ourTagActions["dd"] = new XHTMLTagControlAction(DEFINITION_DESCRIPTION);
    ourTagActions["dfn"] = new XHTMLTagControlAction(DEFINITION);
    ourTagActions["strike"] = new XHTMLTagControlAction(STRIKETHROUGH);

    ourTagActions["a"] = new XHTMLTagHyperlinkAction();
    //ourTagActions["img"] = new XHTMLTagAction();
    ourTagActions["object"] = new XHTMLTagImageAction();

    //ourTagActions["area"] = new XHTMLTagAction();
    //ourTagActions["map"] = new XHTMLTagAction();

    //ourTagActions["base"] = new XHTMLTagAction();
    //ourTagActions["blockquote"] = new XHTMLTagAction();
    ourTagActions["br"] = new XHTMLTagRestartParagraphAction();
    //ourTagActions["center"] = new XHTMLTagAction();
    ourTagActions["div"] = new XHTMLTagParagraphAction();
    //ourTagActions["dt"] = new XHTMLTagAction();
    //ourTagActions["head"] = new XHTMLTagAction();
    //ourTagActions["hr"] = new XHTMLTagAction();
    //ourTagActions["link"] = new XHTMLTagAction();
    //ourTagActions["param"] = new XHTMLTagAction();
    //ourTagActions["q"] = new XHTMLTagAction();
    //ourTagActions["s"] = new XHTMLTagAction();

    ourTagActions["pre"] = new XHTMLTagPreAction();
    //ourTagActions["big"] = new XHTMLTagAction();
    //ourTagActions["small"] = new XHTMLTagAction();
    //ourTagActions["u"] = new XHTMLTagAction();

    //ourTagActions["table"] = new XHTMLTagAction();
    ourTagActions["td"] = new XHTMLTagParagraphAction();
    ourTagActions["th"] = new XHTMLTagParagraphAction();
    //ourTagActions["tr"] = new XHTMLTagAction();
    //ourTagActions["caption"] = new XHTMLTagAction();
    //ourTagActions["span"] = new XHTMLTagAction();
  }
}

XHTMLReader::XHTMLReader(BookReader &modelReader) : myModelReader(modelReader) {
}

bool XHTMLReader::readFile(const std::string &pathPrefix, const std::string &name) {
  fillTagTable();

  myPathPrefix = pathPrefix;
  myFileName = name;
  myModelReader.addHyperlinkLabel(myFileName);

  shared_ptr<ZLInputStream> stream = ZLFile(pathPrefix + name).inputStream();

  myPreformatted = false;

  return readDocument(stream);
}


void XHTMLReader::startElementHandler(const char *tag, const char **attributes) {
  // TODO: tag -> lowercase
  XHTMLTagAction *action = ourTagActions[tag];
  if (action != 0) {
    action->doAtStart(*this, attributes);
  }
}

void XHTMLReader::endElementHandler(const char *tag) {
  // TODO: tag -> lowercase
  XHTMLTagAction *action = ourTagActions[tag];
  if (action != 0) {
    action->doAtEnd(*this);
  }
}

void XHTMLReader::characterDataHandler(const char *text, int len) {
  if (myPreformatted) {
    if ((*text == '\r') || (*text == '\n')) {
      myModelReader.addControl(CODE, false);
      myModelReader.endParagraph();
      myModelReader.beginParagraph();
      myModelReader.addControl(CODE, true);
    }
    // TODO: insert spaces at start of line
    /*
    for (; (len > 0) && isspace(*text); text++, len--) {
      static const std::string NBSP = "\xC0\xA0";
      myModelReader.addData(NBSP);
    }
    */
  }
  myModelReader.addData(std::string(text, len));
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
        }
      }
    }
  }

  return EXTERNAL_DTDs;
}
