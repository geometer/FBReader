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

#ifndef __ZLXMLREADER_H__
#define __ZLXMLREADER_H__

#include <string>
#include <vector>

#include <abstract/shared_ptr.h>

class ZLInputStream;
class ZLXMLReaderInternal;

class ZLXMLReaderBase {

public:
	static size_t bufferSize() { return 2048; }

protected:
  static const char *attributeValue(const char **xmlattributes, const char *name) XML_SECTION;

protected:
	ZLXMLReaderBase(const char *encoding = 0) XML_SECTION;
	virtual ~ZLXMLReaderBase() XML_SECTION;

public:
	bool readDocument(shared_ptr<ZLInputStream> stream) XML_SECTION;

	virtual void startElementHandler(const char *tag, const char **attributes) XML_SECTION = 0;
	virtual void endElementHandler(const char *tag) XML_SECTION = 0;
	virtual void characterDataHandler(const char *text, int len) XML_SECTION = 0;
	virtual const std::vector<std::string> &externalDTDs() const XML_SECTION;

	bool isInterrupted() const XML_SECTION;

protected:
	void interrupt() XML_SECTION;

private:
	bool myInterrupted;
	ZLXMLReaderInternal *myInternalReader;
	char *myParserBuffer;
};

class ZLXMLReader : public ZLXMLReaderBase {

//protected:
public:
	struct Tag {
		const char *tagName;
		int tagCode;
	};

protected:
	ZLXMLReader(const char *encoding = 0) : ZLXMLReaderBase(encoding) {}

	virtual const Tag *tags() const XML_SECTION = 0;

public:
	virtual int tag(const char *name) XML_SECTION;

	virtual void startElementHandler(int tag, const char **attributes) XML_SECTION = 0;
	virtual void endElementHandler(int tag) XML_SECTION = 0;

private:
	void startElementHandler(const char *tag, const char **attributes) XML_SECTION;
	void endElementHandler(const char *tag) XML_SECTION;
};

inline bool ZLXMLReaderBase::isInterrupted() const {
	return myInterrupted;
}

inline void ZLXMLReaderBase::interrupt() {
	myInterrupted = true;
}

#endif /* __ZLXMLREADER_H__ */
