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

#ifndef __XHTMLREADER_H__
#define __XHTMLREADER_H__

#include <string>
#include <map>

#include <abstract/ZLXMLReader.h>

class BookReader;
class XHTMLReader;

class TagAction {

public:
  virtual ~TagAction();
	
  virtual void doAtStart(XHTMLReader &reader, const char **xmlattributes);
  virtual void doAtEnd(XHTMLReader &reader) = 0;
};

class XHTMLReader : public ZLXMLReader {

private:
  static std::map<std::string,TagAction*> ourTagActions;
  static void fillTagTable();

public:
  XHTMLReader(BookReader &modelReader);
  bool readFile(const std::string &pathPrefix, const std::string &name);

  void startElementHandler(const char *tag, const char **attributes) FORMATS_SECTION;
  void endElementHandler(const char *tag) FORMATS_SECTION;
  void characterDataHandler(const char *text, int len) FORMATS_SECTION;

  const std::vector<std::string> &externalDTDs() const XML_SECTION;

private:
  BookReader &myModelReader;
  std::string myFileName;
	bool myPreformatted;

  friend class TagAction;
  friend class TagParagraphAction;
  friend class TagControlAction;
  friend class TagHyperlinkAction;
  friend class TagItemAction;
  friend class TagParagraphWithControlAction;
  friend class TagPreAction;
};

#endif /* __XHTMLREADER_H__ */
