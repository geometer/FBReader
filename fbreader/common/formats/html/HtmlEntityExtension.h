/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __HTMLENTITYEXTENSION_H__
#define __HTMLENTITYEXTENSION_H__

#include <map>

#include <abstract/ZLEncodingConverter.h>
#include <abstract/ZLXMLReader.h>

class HtmlEntityExtension : public ZLControlSequenceExtension {

public:
	static const std::string CollectionFile;
	static int symbolNumber(const std::string &name);

private:
	static std::map<std::string,int> ourSymbolicEntityCollection;

	class CollectionReader : public ZLXMLReader {

	protected:
		const Tag *tags() const FORMATS_SECTION;

	public:
		void startElementHandler(int tag, const char **attributes) FORMATS_SECTION;
		void endElementHandler(int tag) FORMATS_SECTION;
		void characterDataHandler(const char *text, int len) FORMATS_SECTION;
	};

public:
	HtmlEntityExtension() FORMATS_SECTION;
	virtual ~HtmlEntityExtension() FORMATS_SECTION;

	void start() FORMATS_SECTION;
	bool parseCharacter(char ch) FORMATS_SECTION;
	const std::string &buffer() const FORMATS_SECTION;

private:
	std::string myBuffer;

	enum {
		UNDEFINED,
		AS_SYMBOLIC,
		AS_DECIMAL,
		AS_HEXADECIMAL
	} myParseType;
};

inline HtmlEntityExtension::~HtmlEntityExtension() {}
inline void HtmlEntityExtension::start() { myBuffer.erase(); myParseType = UNDEFINED; }
inline const std::string &HtmlEntityExtension::buffer() const { return myBuffer; }

inline void HtmlEntityExtension::CollectionReader::endElementHandler(int) {}
inline void HtmlEntityExtension::CollectionReader::characterDataHandler(const char*, int) {}

#endif /* __HTMLENTITYEXTENSION_H__ */
