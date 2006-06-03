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

#ifndef __HTMLREADER_H__
#define __HTMLREADER_H__

#include <string>
#include <vector>

#include <abstract/ZLEncodingConverter.h>

class ZLInputStream;

class HtmlReader {

public:
  struct HtmlAttribute {
    std::string Name;
    std::string Value;
    bool hasValue;

    HtmlAttribute(const std::string &name) FORMATS_SECTION;
    ~HtmlAttribute() FORMATS_SECTION;
    void setValue(const std::string &value) FORMATS_SECTION;
  };

protected:
  struct HtmlTag {
    std::string Name;
    bool Start;
    std::vector<HtmlAttribute> Attributes;

    HtmlTag() FORMATS_SECTION;
    ~HtmlTag() FORMATS_SECTION;
    void addAttribute(const std::string &name) FORMATS_SECTION;
    void setLastAttributeValue(const std::string &value) FORMATS_SECTION;

  private:
    HtmlTag(const HtmlTag&);
    const HtmlTag &operator= (const HtmlTag&);
  };

private:
  static void setTag(HtmlTag &tag, const std::string &fullName) FORMATS_SECTION;

public:
  void readDocument(ZLInputStream &stream) FORMATS_SECTION;

protected:
  HtmlReader(const std::string &encoding) FORMATS_SECTION;
  virtual ~HtmlReader() FORMATS_SECTION;

protected:
  virtual void startDocumentHandler() FORMATS_SECTION = 0;
  virtual void endDocumentHandler() FORMATS_SECTION = 0;

  virtual bool tagHandler(const HtmlTag &tag) FORMATS_SECTION = 0;
  virtual bool characterDataHandler(const char *text, int len, bool convert) FORMATS_SECTION = 0;

protected:
  shared_ptr<ZLEncodingConverter> myConverter;
};

inline HtmlReader::HtmlAttribute::HtmlAttribute(const std::string &name) : Name(name), hasValue(false) {}
inline HtmlReader::HtmlAttribute::~HtmlAttribute() {}
inline void HtmlReader::HtmlAttribute::setValue(const std::string &value) { Value = value; hasValue = true; }

inline HtmlReader::HtmlTag::HtmlTag() : Start(true) {}
inline HtmlReader::HtmlTag::~HtmlTag() {}
inline void HtmlReader::HtmlTag::addAttribute(const std::string &name) { Attributes.push_back(HtmlAttribute(name)); }
inline void HtmlReader::HtmlTag::setLastAttributeValue(const std::string &value) { if (!Attributes.empty()) Attributes.back().setValue(value); }

#endif /* __HTMLREADER_H__ */
