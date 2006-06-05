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

class ZLXMLReader {

public:
	static size_t bufferSize() { return 2048; }

protected:
  static const char *attributeValue(const char **xmlattributes, const char *name);

protected:
	ZLXMLReader(const char *encoding = 0);
	virtual ~ZLXMLReader();

public:
	bool readDocument(shared_ptr<ZLInputStream> stream);

	virtual void startElementHandler(const char *tag, const char **attributes);
	virtual void endElementHandler(const char *tag);
	virtual void characterDataHandler(const char *text, int len);
	virtual const std::vector<std::string> &externalDTDs() const;

	bool isInterrupted() const;

protected:
	void interrupt();

private:
	bool myInterrupted;
	ZLXMLReaderInternal *myInternalReader;
	char *myParserBuffer;
};

inline bool ZLXMLReader::isInterrupted() const {
	return myInterrupted;
}

inline void ZLXMLReader::interrupt() {
	myInterrupted = true;
}

#endif /* __ZLXMLREADER_H__ */
