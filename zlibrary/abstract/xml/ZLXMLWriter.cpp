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

static const std::string XML_BANNER = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
static const std::string LANGLE = "<";
static const std::string LANGLE_SLASH = "</";
static const std::string RANGLE_EOL = ">\n";
static const std::string SLASH = "/";
static const std::string SPACE = " ";
static const std::string TWO_SPACES = "  ";
static const std::string QUOTE = "\"";
static const std::string EQUALS_QUOTE = "=\"";

void ZLXMLWriter::Tag::writeStart(ZLOutputStream &stream) const {
	stream.write(LANGLE);
	stream.write(myName);
	for (unsigned int i = 0; i < myAttributes.size(); i++) {
		stream.write(SPACE);
		stream.write(myAttributes[i].Name);
		stream.write(EQUALS_QUOTE);
		stream.write(myAttributes[i].Value);
		stream.write(QUOTE);
	}
	if (mySingle) {
		stream.write(SLASH);
	}
	stream.write(RANGLE_EOL);
}

void ZLXMLWriter::Tag::writeEnd(ZLOutputStream &stream) const {
	if (!mySingle) {
		stream.write(LANGLE_SLASH);
		stream.write(myName);
		stream.write(RANGLE_EOL);
	}
}

ZLXMLWriter::ZLXMLWriter(ZLOutputStream &stream) : myStream(stream) {
	stream.write(XML_BANNER);
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
			myStream.write(TWO_SPACES);
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
			myStream.write(TWO_SPACES);
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
