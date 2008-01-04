/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLXMLWRITER_H__
#define __ZLXMLWRITER_H__

#include <stack>
#include <vector>
#include <string>

#include <ZLOutputStream.h>

class ZLXMLWriter {

private:
	struct Attribute {
		std::string Name;
		std::string Value;

		Attribute(const std::string &name, const std::string &value);
		~Attribute();
	};

	class Tag {

	public:
		Tag(const std::string &name, bool single);
		~Tag();
		void addAttribute(const std::string &name, const std::string &value);
		void addData(const std::string &data);
		bool isSingle() const;
		bool isDataEmpty() const;

		void writeStart(ZLOutputStream &stream) const;
		void writeEnd(ZLOutputStream &stream) const;
		
	private:
		std::string myName;
		std::string myData;
		bool mySingle;
		std::vector<Attribute> myAttributes;
	};

protected:
	ZLXMLWriter(ZLOutputStream &stream);
	virtual ~ZLXMLWriter();

	void addTag(const std::string &name, bool single);
	void addAttribute(const std::string &name, const std::string &value);
	void addData(const std::string &data);
	void closeTag();
	void closeAllTags();

private:
	void flushTagStart();

private:
	ZLOutputStream &myStream;
	Tag *myCurrentTag;
	std::stack<Tag*> myTags;
};

inline ZLXMLWriter::Attribute::Attribute(const std::string &name, const std::string &value) : Name(name), Value(value) {}
inline ZLXMLWriter::Attribute::~Attribute() {}

inline bool ZLXMLWriter::Tag::isSingle() const { return mySingle; }
inline bool ZLXMLWriter::Tag::isDataEmpty() const { return myData.empty(); }
inline ZLXMLWriter::Tag::~Tag() {}

inline ZLXMLWriter::~ZLXMLWriter() {}

#endif /* __ZLXMLWRITER_H__ */
