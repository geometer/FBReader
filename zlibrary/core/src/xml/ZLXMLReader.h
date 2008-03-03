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

#ifndef __ZLXMLREADER_H__
#define __ZLXMLREADER_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

class ZLInputStream;
class ZLXMLReaderInternal;

class ZLXMLReader {

public:
  static const char *attributeValue(const char **xmlattributes, const char *name);

protected:
	ZLXMLReader(const char *encoding = 0);
	virtual ~ZLXMLReader();
	const std::map<std::string,std::string> &namespaces() const;

public:
	bool readDocument(shared_ptr<ZLInputStream> stream);
	bool readDocument(const std::string &fileName);

	virtual void startElementHandler(const char *tag, const char **attributes);
	virtual void endElementHandler(const char *tag);
	virtual void characterDataHandler(const char *text, int len);
	virtual bool processNamespaces() const;
	virtual void namespaceListChangedHandler();
	virtual const std::vector<std::string> &externalDTDs() const;

	bool isInterrupted() const;

protected:
	void interrupt();

private:
	bool myInterrupted;
	ZLXMLReaderInternal *myInternalReader;
	char *myParserBuffer;
	std::vector<shared_ptr<std::map<std::string,std::string> > > myNamespaces;

friend class ZLXMLReaderInternal;
};

inline bool ZLXMLReader::isInterrupted() const {
	return myInterrupted;
}

inline void ZLXMLReader::interrupt() {
	myInterrupted = true;
}

#endif /* __ZLXMLREADER_H__ */
