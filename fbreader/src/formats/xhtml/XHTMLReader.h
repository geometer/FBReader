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

#ifndef __XHTMLREADER_H__
#define __XHTMLREADER_H__

#include <string>
#include <map>
#include <stack>

#include <ZLXMLReader.h>

class BookReader;
class XHTMLReader;

class XHTMLTagAction {

public:
	virtual ~XHTMLTagAction();
	
	virtual void doAtStart(XHTMLReader &reader, const char **xmlattributes) = 0;
	virtual void doAtEnd(XHTMLReader &reader) = 0;

protected:
	static BookReader &bookReader(XHTMLReader &reader);	
	const std::string &pathPrefix(XHTMLReader &reader);	
};

class XHTMLReader : public ZLXMLReader {

public:
	static XHTMLTagAction *addAction(const std::string &tag, XHTMLTagAction *action);
	static void fillTagTable();

private:
	static std::map<std::string,XHTMLTagAction*> ourTagActions;

public:
	XHTMLReader(BookReader &modelReader);
	bool readFile(const std::string &pathPrefix, const std::string &fileName, const std::string &referenceName);
	bool readFile(const std::string &pathPrefix, shared_ptr<ZLInputStream> stream, const std::string &referenceName);

	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, int len);

	const std::vector<std::string> &externalDTDs() const;

private:
	BookReader &myModelReader;
	std::string myPathPrefix;
	std::string myReferenceName;
	bool myPreformatted;

	friend class XHTMLTagAction;
	friend class XHTMLTagHyperlinkAction;
	friend class XHTMLTagPreAction;
};

#endif /* __XHTMLREADER_H__ */
