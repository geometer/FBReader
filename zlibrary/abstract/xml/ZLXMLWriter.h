/*
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

#ifndef __ZLXMLWRITER_H__
#define __ZLXMLWRITER_H__

#include <stack>
#include <vector>
#include <string>

#include <abstract/ZLOutputStream.h>

class ZLXMLWriter {

private:
	struct Attribute {
		std::string Name;
		std::string Value;

		Attribute(const std::string &name, const std::string &value) XML_SECTION;
		~Attribute() XML_SECTION;
	};

	class Tag {

	public:
		Tag(const std::string &name, bool single) XML_SECTION;
		~Tag() XML_SECTION;
		void addAttribute(const std::string &name, const std::string &value) XML_SECTION;
		bool isSingle() const XML_SECTION;

		void writeStart(ZLOutputStream &stream) const XML_SECTION;
		void writeEnd(ZLOutputStream &stream) const XML_SECTION;
		
	private:
		std::string myName;
		bool mySingle;
		std::vector<Attribute> myAttributes;
	};

protected:
	ZLXMLWriter(ZLOutputStream &stream) XML_SECTION;
	virtual ~ZLXMLWriter() XML_SECTION;

	void addTag(const std::string &name, bool single) XML_SECTION;
	void addAttribute(const std::string &name, const std::string &value) XML_SECTION;
	void closeTag() XML_SECTION;
	void closeAllTags() XML_SECTION;

private:
	void flushTagStart() XML_SECTION;

private:
	ZLOutputStream &myStream;
	Tag *myCurrentTag;
	std::stack<Tag*> myTags;
};

inline ZLXMLWriter::Attribute::Attribute(const std::string &name, const std::string &value) : Name(name), Value(value) {}
inline ZLXMLWriter::Attribute::~Attribute() {}

inline bool ZLXMLWriter::Tag::isSingle() const { return mySingle; }
inline ZLXMLWriter::Tag::~Tag() {}

inline ZLXMLWriter::~ZLXMLWriter() {}

#endif /* __ZLXMLWRITER_H__ */
