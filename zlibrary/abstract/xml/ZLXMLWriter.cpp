/*
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

#include "ZLXMLWriter.h"

ZLXMLWriter::Tag::Tag(const std::string &name, bool single) : myName(name), mySingle(single) {
}

void ZLXMLWriter::Tag::addAttribute(const std::string &name, const std::string &value) {
	myAttributes.push_back(Attribute(name, value));
}

void ZLXMLWriter::Tag::writeStart(ZLOutputStream &stream) const {
	stream.write("<");
	stream.write(myName.c_str());
	for (unsigned int i = 0; i < myAttributes.size(); i++) {
		stream.write(" ");
		stream.write(myAttributes[i].Name.c_str());
		stream.write("=\"");
		stream.write(myAttributes[i].Value.c_str());
		stream.write("\"");
	}
	if (mySingle) {
		stream.write("/");
	}
	stream.write(">\n");
}

void ZLXMLWriter::Tag::writeEnd(ZLOutputStream &stream) const {
	if (!mySingle) {
		stream.write("</");
		stream.write(myName.c_str());
		stream.write(">\n");
	}
}

ZLXMLWriter::ZLXMLWriter(ZLOutputStream &stream) : myStream(stream) {
	myCurrentTag = 0;
}

void ZLXMLWriter::addTag(const std::string &name, bool single) {
	flushTagStart();
	myCurrentTag = new Tag(name, single);
}

void ZLXMLWriter::addAttribute(const std::string &name, const std::string &value) {
	if (myCurrentTag != 0) {
		myCurrentTag->addAttribute(name, value);
	}
}

void ZLXMLWriter::closeTag() {
	flushTagStart();
	if (myTags.size() > 0) {
		Tag *tag = myTags.top();
		myTags.pop();
		for (unsigned int i = 0; i < myTags.size(); i++) {
			myStream.write("  ");
		}
		tag->writeEnd(myStream);
		delete tag;
	}
}

void ZLXMLWriter::closeAllTags() {
	while (myTags.size() > 0) {
		closeTag();
	}
}

void ZLXMLWriter::flushTagStart() {
	if (myCurrentTag != 0) {
		for (unsigned int i = 0; i < myTags.size(); i++) {
			myStream.write("  ");
		}
		myCurrentTag->writeStart(myStream);
		if (!myCurrentTag->isSingle()) {
			myTags.push(myCurrentTag);
		} else {
			delete myCurrentTag;
		}
		myCurrentTag = 0;
	}
}
