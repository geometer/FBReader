/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSLINKREADER_H__
#define __OPDSLINKREADER_H__

#include <ZLXMLReader.h>

class NetworkLink;

class OPDSLinkReader : public ZLXMLReader {

public:
	OPDSLinkReader();

	shared_ptr<NetworkLink> link();

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

private:
	enum {
		READ_NOTHING,
		READ_SITENAME,
		READ_TITLE,
		READ_SUMMARY,
		READ_LINK,
		READ_ICON_NAME,
		READ_SEARCH_DESCRIPTION,
		READ_SEARCH_PART,
	} myState;
	std::string myLinkType;
	std::string mySearchPartName;

	std::string mySiteName;
	std::string myTitle;
	std::string mySummary;
	std::map<std::string,std::string> myLinks;
	std::string myIconName;

	std::string mySearchType;
	std::map<std::string,std::string> mySearchParts;
};

#endif /* __OPDSLINKREADER_H__ */
