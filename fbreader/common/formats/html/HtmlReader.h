/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __HTMLREADER_H__
#define __HTMLREADER_H__

#include <string>

#include <abstract/ZLXMLReader.h>

class ZLInputStream;

class HtmlTextConverter : public ZLXMLReader {

protected:
	const Tag *tags() const FORMATS_SECTION;

public:
	HtmlTextConverter(const char *encoding) FORMATS_SECTION;
	~HtmlTextConverter() FORMATS_SECTION;
	void convertBuffer(std::vector<std::string> &buffer) FORMATS_SECTION;
	void convertString(std::string &string) FORMATS_SECTION;
	
public:
	void startElementHandler(int tag, const char **attributes) FORMATS_SECTION;
	void endElementHandler(int tag) FORMATS_SECTION;
	void characterDataHandler(const char *text, int len) FORMATS_SECTION;

protected:
	const std::vector<std::string> &externalDTDs() const FORMATS_SECTION;

private:
	std::vector<std::string> *myBuffer;
};

class HtmlReader {

public:
	enum TagCode {
		_UNKNOWN,
		_HEAD,
		_BODY,
		_TITLE,
		_H1, _H2, _H3, _H4, _H5, _H6,
		// 9. text
		_EM, _STRONG, _DFN, _CODE, _SAMP, _KBD, _VAR, _CITE, _ABBR, _ACRONYM,
		_BLOCKQUOTE, _Q,
		_SUB, _SUP,
		_P, _BR,
		_PRE,
		_INS, _DEL,
		// 10. lists
		_UL, _OL, _LI,
		_DL, _DT, _DD,
		//
		_TT,
		_B, _I,
		_STYLE,
		_A,
		_SCRIPT,
	};

protected:
	struct HtmlAttribute {
		std::string Name;
		std::string Value;
		bool hasValue;

		HtmlAttribute(const std::string &name) FORMATS_SECTION;
		~HtmlAttribute() FORMATS_SECTION;
		void setValue(const std::string &value) FORMATS_SECTION;
	};

	struct HtmlTag {
		TagCode Code;
		bool Start;
		std::vector<HtmlAttribute> Attributes;

		HtmlTag(TagCode code, bool start) FORMATS_SECTION;
		~HtmlTag() FORMATS_SECTION;
		void addAttribute(const std::string &name) FORMATS_SECTION;
		void setLastAttributeValue(const std::string &value) FORMATS_SECTION;
	};

private:
	static HtmlTag tag(std::string &name) FORMATS_SECTION;

public:
	void readDocument(ZLInputStream &stream, const std::string &encoding) FORMATS_SECTION;

protected:
	HtmlReader() FORMATS_SECTION;
	virtual ~HtmlReader() FORMATS_SECTION;

protected:
	virtual void startDocumentHandler() FORMATS_SECTION = 0;
	virtual void endDocumentHandler() FORMATS_SECTION = 0;

	virtual bool tagHandler(HtmlTag tag) FORMATS_SECTION = 0;
	virtual bool characterDataHandler(const char *text, int len) FORMATS_SECTION = 0;

protected:
	HtmlTextConverter *myConverter;
};

inline HtmlTextConverter::HtmlTextConverter(const char *encoding) : ZLXMLReader(encoding) {}
inline HtmlTextConverter::~HtmlTextConverter() {}

inline HtmlReader::HtmlAttribute::HtmlAttribute(const std::string &name) : Name(name), hasValue(false) {}
inline HtmlReader::HtmlAttribute::~HtmlAttribute() {}
inline void HtmlReader::HtmlAttribute::setValue(const std::string &value) { Value = value; hasValue = true; }

inline HtmlReader::HtmlTag::HtmlTag(TagCode code, bool start) : Code(code), Start(start) {}
inline HtmlReader::HtmlTag::~HtmlTag() {}
inline void HtmlReader::HtmlTag::addAttribute(const std::string &name) { Attributes.push_back(HtmlAttribute(name)); }
inline void HtmlReader::HtmlTag::setLastAttributeValue(const std::string &value) { if (!Attributes.empty()) Attributes.back().setValue(value); }

#endif /* __HTMLREADER_H__ */
